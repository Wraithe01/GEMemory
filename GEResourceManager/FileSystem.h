#pragma once

#include "Includes.h"

typedef uint32_t FILEid;

struct AsyncFileRequestHandle
{
	uint32_t requestid = 0;
	bool requestServed = false;
	uint32_t error = 0;
	size_t returnValue = -1;
};

typedef void (*FileCallbackFunction)(AsyncFileRequestHandle request);

enum AsyncFileRequestType
{
	None,
	AsyncRead,
	AsyncWrite,
	AgentTermination
};

struct AsyncFileRequest
{
	AsyncFileRequestType type = AsyncFileRequestType::None;
	FILEid file;
	void* buffer = nullptr;
	size_t elementSize = 0;
	size_t elementCount = 0;
	FileCallbackFunction callback = nullptr;
	AsyncFileRequestHandle* handle = nullptr;
};

class FileSystem
{
public:
	FileSystem() = delete;
	FileSystem(const char* rootPath, uint32_t asyncAgentThreads);
	~FileSystem();

	virtual FILEid Open(const char* path, const char* mode) = 0;

	virtual int Close(FILEid file) = 0;

	virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount, FILEid file) = 0;

	virtual size_t Write(const void* buffer, size_t elementSize, size_t elementCount, FILEid file) = 0;

	
	FILEid AsyncOpen(const char* path, const char* mode);

	int AsyncClose(FILEid file);

	void AsyncReadRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, AsyncFileRequestHandle* requestHandle);
	
	void AsyncWriteRequest(const void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, AsyncFileRequestHandle* requestHandle);

	bool AsyncRequestSucceeded(const AsyncFileRequestHandle& request);
	
	void AsynchRequestWait(const AsyncFileRequestHandle& request);

	size_t AsyncGetBytesReadOrWritten(const AsyncFileRequestHandle& request);

protected:
	const char* m_rootPath;

private:
	void RequestAgent();

	void EnqueueRequest(AsyncFileRequest* request);

	uint32_t m_requestAgents;

	std::queue<AsyncFileRequest> m_requestQueue;
	std::mutex m_queueLock;
	std::condition_variable m_dequeueCnd;

	std::mutex m_waitLock;
	std::condition_variable m_waitCnd;

};
