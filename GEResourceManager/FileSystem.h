#pragma once

#include "Includes.h"
#include "AsyncFunctionality.h"

// fopen based function call
#define FileOpen(path, mode) CFileSystem::Instance()->Open(path, mode)
// fclose based function call
#define FileClose(FILEid) CFileSystem::Instance()->Close(FILEid)
// fread based function call. reads elementCount number of elements of size elementSize to buffer
#define FileRead(buffer, elementSize, elementCount, file) CFileSystem::Instance()->Read(buffer, elementSize, elementCount, file)
// fwrite based function call. writes elementCount number of elements of size elementSize from buffer
#define FileWrite(buffer, elementSize, elementCount, file) CFileSystem::Instance()->Write(buffer, elementSize, elementCount, file)
// fseek based function call.
#define FileSeek(file, offset, origin) CFileSystem::Instance()->Seek(file, offset, origin)

// Async file open. Call with same input as fopen.
// Pass optional callback function or nullptr.
// A handle for the request is returned that can be waited for.
#define FileOpenAsync(path, mode, callback, callbackInput) CFileSystem::Instance()->AsyncOpenRequest(path, mode, callback, callbackInput)
// Async file close. Call with same input as fclose.
// Pass optional callback function or nullptr.
// A handle for the request is returned that can be waited for.
#define FileCloseAsync(FILEid, callback, callbackInput) CFileSystem::Instance()->AsyncCloseRequest(FILEid, callback, callbackInput)
// Async file reading. Call with same input as fread.
// Pass optional callback function or nullptr.
// A handle for the request is returned that can be waited for.
#define FileReadAsync(buffer, elementSize, elementCount, file, callback, callbackInput) CFileSystem::Instance()->AsyncReadRequest(buffer, elementSize, elementCount, file, callback, callbackInput)
// Async file writing. Call with same input as fwrite.
// Pass optional callback function or nullptr.
// A handle for the request is returned that can be waited for.
#define FileWriteAsync(buffer, elementSize, elementCount, file, callback, callbackInput) CFileSystem::Instance()->AsyncWriteRequest(buffer, elementSize, elementCount, file, callback, callbackInput)
// Async file seek. Call with same input as fseek.
// Pass optional callback function or nullptr.
// A handle for the request is returned that can be waited for.
#define FileSeekAsync(file, offset, origin, callback, callbackInput) CFileSystem::Instance()->AsyncSeekRequest(file, offset, origin, callback, callbackInput)

// Blocks until request has been completed
#define FileRequestWait(request) CFileSystem::Instance()->AsynchRequestWait(request)
// Unblocking check if request is completed
#define FileRequestFinished(request) CFileSystem::Instance()->AsynchRequestCompleted(request)
// Blocks until callback function has terminated
#define FileRequestCallbackWait(request) CFileSystem::Instance()->AsynchCallbackWait(request)
// Unblocking check if callback function is terminated
#define FileRequestCallbackFinished(request) CFileSystem::Instance()->AsynchCallbackCompleted(request)
// Checks if request was completed without error
#define FileRequestSucceded(request) CFileSystem::Instance()->AsyncRequestSucceeded(request)
// Returns how many bytes read or written with read and write requests
#define FileRequestBytesReadOrWritten(request) CFileSystem::Instance()->AsyncGetBytesReadOrWritten(request)
// Returns fileID from FileOpen request
#define FileRequestGetFileID(request) CFileSystem::Instance()->AsyncGetRequestFileID(request)

typedef int FILEid;

struct AsyncFileRequestOUT
{
	bool error = false;
	size_t returnValue = -1;
	FILEid file = 0;
};

typedef AsyncRequestHandle<AsyncFileRequestOUT> AsyncFileRequestHandle;

typedef void (*FileCallbackFunction)(AsyncFileRequestHandle request, void* callbackInput);

enum AsyncFileRequestType
{
	None,
	AsyncOpen,
	AsyncClose,
	AsyncRead,
	AsyncWrite,
	AsyncSeek
};

enum SeekOrigin
{
	Start,
	Current,
	End,
	OriginNone
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
	long offset = 0;
	SeekOrigin origin = SeekOrigin::OriginNone;
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

	virtual int Seek(FILEid file, long offset, SeekOrigin origin) = 0;
	
	AsyncFileRequestHandle AsyncOpenRequest(const char* path, const char* mode, FileCallbackFunction callback, void* callbackInput);

	AsyncFileRequestHandle AsyncCloseRequest(FILEid file, FileCallbackFunction callback, void* callbackInput);

	AsyncFileRequestHandle AsyncReadRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, void* callbackInput);
	
	AsyncFileRequestHandle AsyncWriteRequest(void* buffer, size_t elementSize, size_t elementCount, FILEid file, FileCallbackFunction callback, void* callbackInput);

	AsyncFileRequestHandle AsyncSeekRequest(FILEid file, long offset, SeekOrigin origin, FileCallbackFunction callback, void* callbackInput);

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

	virtual int Seek(FILEid file, long offset, SeekOrigin origin) override;
};
