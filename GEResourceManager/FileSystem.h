#pragma once

#include "Includes.h"
#include "AsyncFunctionality.h"

#define FileOpen(path, mode) CFileSystem::Instance()->Open(path, mode)
#define FileClose(FILEid) CFileSystem::Instance()->Close(FILEid)
#define FileRead(buffer, elementSize, elementCount, file) CFileSystem::Instance()->Read(buffer, elementSize, elementCount, file)
#define FileWrite(buffer, elementSize, elementCount, file) CFileSystem::Instance()->Write(buffer, elementSize, elementCount, file)

typedef int FILEid;

struct AsyncFileRequestOUT
{
	bool error = false;
	size_t returnValue = -1;
	FILEid file = 0;
};

typedef AsyncRequestHandle<AsyncFileRequestOUT> AsyncFileRequestHandle;

typedef void (*FileCallbackFunction)(AsyncFileRequestHandle request);

enum AsyncFileRequestType
{
	None,
	AsyncOpen,
	AsyncClose,
	AsyncRead,
	AsyncWrite
};

struct AsyncFileRequestIN
{
	AsyncFileRequestType type = AsyncFileRequestType::None;
	FILEid file;
	void* buffer = nullptr;
	const char* path = "";
	const char* mode = "";
	size_t elementSize = 0;
	size_t elementCount = 0;
};

class FileSystem : public AsyncFunctionality<AsyncFileRequestIN, AsyncFileRequestOUT>
{
public:
	FileSystem() = delete;
	FileSystem(uint32_t asyncAgentThreads);

	virtual FILEid Open(const char* path, const char* mode) = 0;

	virtual int Close(FILEid file) = 0;

	virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount, FILEid file) = 0;

	virtual size_t Write(const void* buffer, size_t elementSize, size_t elementCount, FILEid file) = 0;
	
	//FILEid AsyncOpen(const char* path, const char* mode);
	AsyncFileRequestHandle AsyncOpenRequest(const char* path, const char* mode, FileCallbackFunction callback);

	AsyncFileRequestHandle AsyncCloseRequest(FILEid file, FileCallbackFunction callback);

	AsyncFileRequestHandle AsyncReadRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback);
	
	AsyncFileRequestHandle AsyncWriteRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback);

	bool AsyncRequestSucceeded(const AsyncFileRequestHandle request);

	size_t AsyncGetBytesReadOrWritten(const AsyncFileRequestHandle request);

	FILEid AsyncGetRequestFileID(const AsyncFileRequestHandle request);

protected:
	virtual void HandleRequest(const AsyncFileRequestIN& requestIN, AsyncFileRequestOUT* o_requestOUT) override;
};

class CFileSystem : public FileSystem
{
private:
	static CFileSystem* instancePtr;
	CFileSystem();

	std::map<FILEid, FILE*> m_fileptrs;
	FILEid m_lastID;

	std::mutex m_mapLock;

public:
	CFileSystem(const CFileSystem& obj) = delete;
	~CFileSystem();

	static CFileSystem* Instance();

	virtual FILEid Open(const char* path, const char* mode) override;

	virtual int Close(FILEid file) override;

	virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount, FILEid file) override;

	virtual size_t Write(const void* buffer, size_t elementSize, size_t elementCount, FILEid file) override;
};
