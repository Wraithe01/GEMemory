#include "FileSystem.h"

FileSystem::FileSystem(uint32_t asyncAgentThreads) :
	AsyncFunctionality(asyncAgentThreads)
{
}

AsyncFileRequestHandle FileSystem::AsyncOpenRequest(const char* path, const char* mode, FileCallbackFunction callback, void* callbackInput)
{
	AsyncFileRequestIN request = 
	{
		AsyncFileRequestType::AsyncOpen,
		-1,
		nullptr,
		path,
		mode,
		0,
		0,
		0,
		SeekOrigin::OriginNone
	};
	return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncCloseRequest(FILEid file, FileCallbackFunction callback, void* callbackInput)
{
	AsyncFileRequestIN request =
	{
		AsyncFileRequestType::AsyncClose,
		file,
		nullptr,
		"",
		"",
		0,
		0,
		0,
		SeekOrigin::OriginNone
	};
	return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncReadRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, void* callbackInput)
{
	AsyncFileRequestIN request =
	{
		AsyncFileRequestType::AsyncRead,
		file,
		buffer,
		"",
		"",
		elementSize,
		elementCount,
		0,
		SeekOrigin::OriginNone
	};
	return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncWriteRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, void* callbackInput)
{
	AsyncFileRequestIN request =
	{
		AsyncFileRequestType::AsyncWrite,
		file,
		buffer,
		"",
		"",
		elementSize,
		elementCount,
		0,
		SeekOrigin::OriginNone
	};
	return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncSeekRequest(FILEid file, long offset, SeekOrigin origin, FileCallbackFunction callback, void* callbackInput)
{
	AsyncFileRequestIN request =
	{
		AsyncFileRequestType::AsyncSeek,
		file,
		nullptr,
		"",
		"",
		0,
		0,
		offset,
		origin
	};
	return EnqueueRequest(request, callback, callbackInput);
}

bool FileSystem::AsyncRequestSucceeded(const AsyncFileRequestHandle request)
{
	return !(ReturnDataFromHandle(request)->error);
}

size_t FileSystem::AsyncGetBytesReadOrWritten(const AsyncFileRequestHandle request)
{
	return ReturnDataFromHandle(request)->returnValue;
}

FILEid FileSystem::AsyncGetRequestFileID(const AsyncFileRequestHandle request)
{
	return ReturnDataFromHandle(request)->file;
}

void FileSystem::HandleRequest(const AsyncFileRequestIN& requestIN, AsyncFileRequestOUT* o_requestOUT)
{
	switch (requestIN.type)
	{
	case AsyncFileRequestType::AsyncOpen:
		o_requestOUT->file = Open(requestIN.path, requestIN.mode);
		if (o_requestOUT->file < 0)
		{
			o_requestOUT->error = true;
		}
		break;
	case AsyncFileRequestType::AsyncClose:
		if (Close(requestIN.file) != 0)
		{
			o_requestOUT->error = true;
		}
		break;
	case AsyncFileRequestType::AsyncRead:
		o_requestOUT->returnValue = Read(requestIN.buffer, requestIN.elementSize, requestIN.elementCount, requestIN.file);
		break;
	case AsyncFileRequestType::AsyncWrite:
		o_requestOUT->returnValue = Write(requestIN.buffer, requestIN.elementSize, requestIN.elementCount, requestIN.file);
		break;
	case AsyncFileRequestType::AsyncSeek:
		if (Seek(requestIN.file, requestIN.offset, requestIN.origin) != 0)
		{
			o_requestOUT->error = true;
		}
		break;
	}
}

CFileSystem::CFileSystem() :
	FileSystem(FILESYSTEM_ASYNCTHREADS),
	m_lastID(0)
{
}

CFileSystem::~CFileSystem()
{
	for (auto const& pair : m_fileptrs)
	{
		fclose(pair.second);
	}
}

CFileSystem* CFileSystem::Instance()
{
	if (instancePtr == NULL)
	{
		instancePtr = new CFileSystem();
		return instancePtr;
	}
	else
	{
		return instancePtr;
	}
}
CFileSystem* CFileSystem::instancePtr = NULL;

FILEid CFileSystem::Open(const char* path, const char* mode)
{
	m_mapLock.lock();
	if (m_lastID < 0)
	{
		m_lastID = 0;
	}
	while (m_fileptrs.count(m_lastID) > 0)
	{
		m_lastID++;
	}
	FILE* ptr;
	errno_t err;
	if ((err = fopen_s(&ptr, path, mode)) != 0)
	{
		std::cerr << "Error " << err << " when opening file " << path << std::endl;
	}
	if (ptr != nullptr)
	{
		m_fileptrs[m_lastID] = ptr;
		m_mapLock.unlock();
		return m_lastID;
	}
	m_mapLock.unlock();
	return -1;
}

int CFileSystem::Close(FILEid file)
{
	m_mapLock.lock();
	if (m_fileptrs.count(file) == 0)
	{
		m_mapLock.unlock();
		return -1;
	}
	FILE* ptr = m_fileptrs[file];
	m_fileptrs.erase(file);
	m_mapLock.unlock();
	return fclose(ptr);
}

size_t CFileSystem::Read(void* buffer, size_t elementSize, size_t elementCount, FILEid file)
{
	if (m_fileptrs.count(file) == 0)
	{
		return 0;
	}
	return fread(buffer, elementSize, elementCount, m_fileptrs[file]);
}

size_t CFileSystem::Write(const void* buffer, size_t elementSize, size_t elementCount, FILEid file)
{
	if (m_fileptrs.count(file) == 0)
	{
		return 0;
	}
	return fwrite(buffer, elementSize, elementCount, m_fileptrs[file]);
}

int CFileSystem::Seek(FILEid file, long offset, SeekOrigin origin)
{
	if (m_fileptrs.count(file) == 0)
	{
		return -1;
	}
	switch (origin)
	{
	case SeekOrigin::Start:
		return fseek(m_fileptrs[file], offset, SEEK_SET);
		break;
	case SeekOrigin::Current:
		return fseek(m_fileptrs[file], offset, SEEK_CUR);
		break;
	case SeekOrigin::End:
		return fseek(m_fileptrs[file], offset, SEEK_END);
		break;
	}
	return -1;
}
