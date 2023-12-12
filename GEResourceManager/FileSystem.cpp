#include "FileSystem.h"

FileSystem::FileSystem(uint32_t asyncAgentThreads)
{
	m_agentThreads = new std::thread[asyncAgentThreads];

	for (int i = 0; i < asyncAgentThreads; i++)
	{
		m_agentThreads[i] = std::thread(&FileSystem::RequestThread, this);
	}
}

FileSystem::~FileSystem()
{
	AsyncFileRequest terminationRequest;
	terminationRequest.type = AsyncFileRequestType::AgentTermination;
	
	for (int i = 0; i < m_requestAgents; i++)
	{
		EnqueueRequest(&terminationRequest);
	}
	for (int i = 0; i < m_requestAgents; i++)
	{
		m_agentThreads[i].join();
	}
	delete[] m_agentThreads;
}

AsyncFileRequestHandle FileSystem::AsyncOpenRequest(const char* path, const char* mode, FileCallbackFunction callback)
{
	AsyncFileRequestHandle requestHandle(new AsyncFileRequestStatus);
	AsyncFileRequest request =
	{
		AsyncFileRequestType::AsyncOpen,
		0,
		nullptr,
		path,
		mode,
		0,
		0,
		callback,
		AsyncFileRequestHandle(requestHandle)
	};
	EnqueueRequest(&request);
	return requestHandle;
}

AsyncFileRequestHandle FileSystem::AsyncCloseRequest(FILEid file, FileCallbackFunction callback)
{
	AsyncFileRequestHandle requestHandle(new AsyncFileRequestStatus);
	AsyncFileRequest request =
	{
		AsyncFileRequestType::AsyncClose,
		file,
		nullptr,
		"",
		"",
		0,
		0,
		callback,
		AsyncFileRequestHandle(requestHandle)
	};
	EnqueueRequest(&request);
	return requestHandle;
}

AsyncFileRequestHandle FileSystem::AsyncReadRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback)
{
	AsyncFileRequestHandle requestHandle(new AsyncFileRequestStatus);
	AsyncFileRequest request =
	{
		AsyncFileRequestType::AsyncRead,
		file,
		buffer,
		"",
		"",
		elementSize,
		elementCount,
		callback,
		AsyncFileRequestHandle(requestHandle)
	};
	EnqueueRequest(&request);
	return requestHandle;
}

AsyncFileRequestHandle FileSystem::AsyncWriteRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback)
{
	AsyncFileRequestHandle requestHandle(new AsyncFileRequestStatus);
	AsyncFileRequest request =
	{
		AsyncFileRequestType::AsyncWrite,
		file,
		buffer,
		"",
		"",
		elementSize,
		elementCount,
		callback,
		AsyncFileRequestHandle(requestHandle)
	};
	EnqueueRequest(&request);
	return requestHandle;
}

bool FileSystem::AsyncRequestSucceeded(const AsyncFileRequestHandle request)
{
	return request->requestServed && (!request->error);
}

void FileSystem::AsynchRequestWait(const AsyncFileRequestHandle request)
{
	std::unique_lock<std::mutex> cndLock(request->handleLock);
	while (!request->requestServed) request->waitCnd.wait(cndLock);
	cndLock.unlock();
}

bool FileSystem::AsynchRequestCheck(const AsyncFileRequestHandle request)
{
	return request->requestServed;
}

size_t FileSystem::AsyncGetBytesReadOrWritten(const AsyncFileRequestHandle request)
{
	return request->returnValue;
}

FILEid FileSystem::AsyncGetRequestFileID(const AsyncFileRequestHandle request)
{
	return request->file;
}

void FileSystem::RequestThread()
{
	m_agentsTallyLock.lock();
	m_requestAgents++;
	m_agentsTallyLock.unlock();

	AsyncFileRequest request;
	std::unique_lock<std::mutex> cndLock(m_queueLock);
	while (true)
	{
		while (m_requestQueue.empty()) m_dequeueCnd.wait(cndLock);

		request = m_requestQueue.front();
		m_requestQueue.pop();

		cndLock.unlock();
		if (request.type == AsyncFileRequestType::AsyncOpen)
		{
			request.handle->file = Open(request.path, request.mode);
			if (request.handle->file < 0)
			{
				request.handle->error = true;
			}
			PostRequest(&request);
		}
		else if (request.type == AsyncFileRequestType::AsyncClose)
		{
			if (Close(request.file) != 0)
			{
				request.handle->error = true;
			}
			PostRequest(&request);
		}
		else if (request.type == AsyncFileRequestType::AsyncRead)
		{
			request.handle->returnValue = Read(request.buffer, request.elementSize, request.elementCount, request.file);
			PostRequest(&request);
		}
		else if (request.type == AsyncFileRequestType::AsyncWrite)
		{
			request.handle->returnValue = Write(request.buffer, request.elementSize, request.elementCount, request.file);
			if (request.handle->returnValue < request.elementCount)
			{
				request.handle->error = true;
			}
			PostRequest(&request);
		}
		else if (request.type == AsyncFileRequestType::AgentTermination)
		{
			break;
		}
		cndLock.lock();
	}
}

void FileSystem::EnqueueRequest(AsyncFileRequest* request)
{
	// enqueues request
	m_queueLock.lock();
	m_requestQueue.push(*request);
	m_queueLock.unlock();
	// wakes queue agent if waiting
	m_dequeueCnd.notify_one();
}

void FileSystem::PostRequest(AsyncFileRequest* request)
{
	// notify and post original thread
	request->handle->handleLock.lock();
	request->handle->requestServed = true;
	request->handle->handleLock.unlock();
	request->handle->waitCnd.notify_all();

	// Calls callback function
	if (request->callback != nullptr)
	{
		request->callback(request->handle);
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
