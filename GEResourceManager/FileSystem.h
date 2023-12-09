#pragma once

#include "Includes.h"

typedef int FILEid;

struct AsyncFileRequestStatus
{
	bool requestServed = false;
	std::mutex handleLock;
	std::condition_variable waitCnd;

	bool error = false;
	size_t returnValue = -1;
	FILEid file = 0;
};

typedef std::shared_ptr<AsyncFileRequestStatus> AsyncFileRequestHandle;

typedef void (*FileCallbackFunction)(AsyncFileRequestHandle request);

enum AsyncFileRequestType
{
	None,
	AsyncOpen,
	AsyncClose,
	AsyncRead,
	AsyncWrite,
	AgentTermination
};

struct AsyncFileRequest
{
	AsyncFileRequestType type = AsyncFileRequestType::None;
	FILEid file;
	void* buffer = nullptr;
	const char* path = "";
	const char* mode = "";
	size_t elementSize = 0;
	size_t elementCount = 0;
	FileCallbackFunction callback = nullptr;
	AsyncFileRequestHandle handle = nullptr;
};

class FileSystem
{
public:
	FileSystem() = delete;
	FileSystem(uint32_t asyncAgentThreads);
	~FileSystem();

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
	
	void AsynchRequestWait(const AsyncFileRequestHandle request);

	bool AsynchRequestCheck(const AsyncFileRequestHandle request);

	size_t AsyncGetBytesReadOrWritten(const AsyncFileRequestHandle request);

	FILEid AsyncGetRequestFileID(const AsyncFileRequestHandle request);

	void RequestThread();

private:

	void EnqueueRequest(AsyncFileRequest* request);
	void PostRequest(AsyncFileRequest* request);

	std::thread* m_agentThreads;
	uint32_t m_requestAgents;
	std::mutex m_agentsTallyLock;

	std::queue<AsyncFileRequest> m_requestQueue;
	std::mutex m_queueLock;
	std::condition_variable m_dequeueCnd;
};

class CFileSystem : public FileSystem
{
public:
	CFileSystem();
	CFileSystem(uint32_t asyncAgentThreads);
	~CFileSystem();

	virtual FILEid Open(const char* path, const char* mode) override;

	virtual int Close(FILEid file) override;

	virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount, FILEid file) override;

	virtual size_t Write(const void* buffer, size_t elementSize, size_t elementCount, FILEid file) override;

private:
	std::map<FILEid, FILE*> m_fileptrs;
	FILEid m_lastID;

	std::mutex m_mapLock;
};
