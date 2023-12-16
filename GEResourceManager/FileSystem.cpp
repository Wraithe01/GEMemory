#include "FileSystem.h"

FileSystem::FileSystem(uint32_t asyncAgentThreads)
: AsyncFunctionality(asyncAgentThreads)
{
}

AsyncFileRequestHandle FileSystem::AsyncOpenRequest(const char*          path,
                                                    const char*          mode,
                                                    FileCallbackFunction callback,
                                                    void*                callbackInput)
{
    AsyncFileRequestIN request
        = { AsyncFileRequestType::AsyncOpen, -1, nullptr, path, mode, 0, 0, 0,
            SeekOrigin::OriginNone };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncCloseRequest(FILEid               file,
                                                     FileCallbackFunction callback,
                                                     void*                callbackInput)
{
    AsyncFileRequestIN request = {
        AsyncFileRequestType::AsyncClose, file, nullptr, "", "", 0, 0, 0, SeekOrigin::OriginNone
    };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncReadRequest(void*                buffer,
                                                    size_t               elementSize,
                                                    size_t               elementCount,
                                                    FILEid               file,
                                                    FileCallbackFunction callback,
                                                    void*                callbackInput)
{
    AsyncFileRequestIN request
        = { AsyncFileRequestType::AsyncRead, file, buffer, "", "", elementSize, elementCount, 0,
            SeekOrigin::OriginNone };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncWriteRequest(void*                buffer,
                                                     size_t               elementSize,
                                                     size_t               elementCount,
                                                     FILEid               file,
                                                     FileCallbackFunction callback,
                                                     void*                callbackInput)
{
    AsyncFileRequestIN request = { AsyncFileRequestType::AsyncWrite,
                                   file,
                                   buffer,
                                   "",
                                   "",
                                   elementSize,
                                   elementCount,
                                   0,
                                   SeekOrigin::OriginNone };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncSeekRequest(
    FILEid file, long offset, SeekOrigin origin, FileCallbackFunction callback, void* callbackInput)
{
    AsyncFileRequestIN request
        = { AsyncFileRequestType::AsyncSeek, file, nullptr, "", "", 0, 0, offset, origin };
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

AsyncFileRequestHandle FileSystem::AsyncPakOpenRequest(const char*          path,
                                                       FileCallbackFunction callback,
                                                       void*                callbackInput)
{
    AsyncFileRequestIN request = {
        AsyncFileRequestType::AsyncPakOpen, -1, nullptr, path, "", 0, 0, 0, SeekOrigin::OriginNone
    };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncPakCloseRequest(PAKid                package,
                                                        FileCallbackFunction callback,
                                                        void*                callbackInput)
{
    AsyncFileRequestIN request = { AsyncFileRequestType::AsyncPakClose,
                                   -1,
                                   nullptr,
                                   "",
                                   "",
                                   0,
                                   0,
                                   0,
                                   SeekOrigin::OriginNone,
                                   package };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncPakSeekFileRequest(PAKid                package,
                                                           FilePos              position,
                                                           FileCallbackFunction callback,
                                                           void*                callbackInput)
{
    AsyncFileRequestIN request = { AsyncFileRequestType::AsyncPakSeek,
                                   -1,
                                   nullptr,
                                   "",
                                   "",
                                   0,
                                   0,
                                   0,
                                   SeekOrigin::OriginNone,
                                   package,
                                   position };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncPakOpenCurrentFileRequest(PAKid                package,
                                                                  FileCallbackFunction callback,
                                                                  void* callbackInput)
{
    AsyncFileRequestIN request = {
        AsyncFileRequestType::AsyncPakFileOpen,
        -1,
        nullptr,
        "",
        "",
        0,
        0,
        0,
        SeekOrigin::OriginNone,
        package,
    };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncPakCloseCurrentFileRequest(PAKid                package,
                                                                   FileCallbackFunction callback,
                                                                   void* callbackInput)
{
    AsyncFileRequestIN request = {
        AsyncFileRequestType::AsyncPakFileClose,
        -1,
        nullptr,
        "",
        "",
        0,
        0,
        0,
        SeekOrigin::OriginNone,
        package,
    };
    return EnqueueRequest(request, callback, callbackInput);
}

AsyncFileRequestHandle FileSystem::AsyncPakCurrentFileReadRequest(
    void* buffer, uint32_t bytes, PAKid package, FileCallbackFunction callback, void* callbackInput)
{
    AsyncFileRequestIN request = { AsyncFileRequestType::AsyncPakFileRead,
                                   -1,
                                   buffer,
                                   "",
                                   "",
                                   0,
                                   bytes,
                                   0,
                                   SeekOrigin::OriginNone,
                                   package };
    return EnqueueRequest(request, callback, callbackInput);
}

PAKid FileSystem::AsyncGetRequestPakID(const AsyncFileRequestHandle request,
                                       FileCallbackFunction         callback,
                                       void*                        callbackInput)
{
    return ReturnDataFromHandle(request)->package;
}

AsyncFileRequestHandle FileSystem::AsyncCustomFileRequest(FileCallbackFunction callback,
                                                          void*                callbackInput)
{
    AsyncFileRequestIN request = { AsyncFileRequestType::AsyncGeneralWork };
    return EnqueueRequest(request, callback, callbackInput);
}

void FileSystem::HandleRequest(const AsyncFileRequestIN& requestIN,
                               AsyncFileRequestOUT*      o_requestOUT)
{
    int temp = 0;
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
            o_requestOUT->returnValue = Read(
                requestIN.buffer, requestIN.elementSize, requestIN.elementCount, requestIN.file);
            break;
        case AsyncFileRequestType::AsyncWrite:
            o_requestOUT->returnValue = Write(
                requestIN.buffer, requestIN.elementSize, requestIN.elementCount, requestIN.file);
            break;
        case AsyncFileRequestType::AsyncSeek:
            if (Seek(requestIN.file, requestIN.offset, requestIN.origin) != 0)
            {
                o_requestOUT->error = true;
            }
            break;

        case AsyncFileRequestType::AsyncPakOpen:
            o_requestOUT->package = PakOpen(requestIN.path);
            if (o_requestOUT->package.handle == nullptr)
            {
                o_requestOUT->error = true;
            }
            break;
        case AsyncFileRequestType::AsyncPakClose:
            if (PakClose(requestIN.package) != 0)
            {
                o_requestOUT->error = true;
            }
            break;
        case AsyncFileRequestType::AsyncPakSeek:
            if (PakSeekFile(requestIN.package, requestIN.filePos) != 0)
            {
                o_requestOUT->error = true;
            }
            break;
        case AsyncFileRequestType::AsyncPakFileOpen:
            if (PakOpenCurrentFile(requestIN.package) != 0)
            {
                o_requestOUT->error = true;
            }
            break;
        case AsyncFileRequestType::AsyncPakFileClose:
            if (PakCloseCurrentFile(requestIN.package) != 0)
            {
                o_requestOUT->error = true;
            }
            break;
        case AsyncFileRequestType::AsyncPakFileRead:
            temp = PakCurrentFileRead(requestIN.buffer, requestIN.elementCount, requestIN.package);
            if (temp < 0)
            {
                o_requestOUT->error = true;
            }
            else
            {
                o_requestOUT->returnValue = temp;
            }
            break;
        case AsyncFileRequestType::AsyncGeneralWork:
            break;
    }
}

CFileSystem::CFileSystem()
: FileSystem(FILESYSTEM_ASYNCTHREADS)
, m_lastID(0)
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
    FILE*   ptr;
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

bool CFileSystem::WasOpened(FILEid file) { return file >= 0; }

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

PAKid FileSystem::PakOpen(const char* path) { return { 0, unzOpen(path) }; }

bool FileSystem::PakWasOpened(PAKid package)
{
    return package.handle != nullptr;
}


int FileSystem::PakClose(PAKid package)
{
    if (package.format == 0)
        return unzClose(package.handle);
    return -1;
}

int FileSystem::PakSeekFile(PAKid package, FilePos position)
{
    if (package.format == 0)
        return unzGoToFilePos(package.handle, &position);
    return -1;
}

PakFileInfo FileSystem::PakGetCurrentFileInfo(PAKid package)
{
    if (package.format == 0)
    {
        unz_file_info info;
        unzGetCurrentFileInfo(package.handle, &info, nullptr, 0, nullptr, 0, nullptr, 0);
        return { info.uncompressed_size };
    }
    return PakFileInfo();
}

int FileSystem::PakOpenCurrentFile(PAKid package)
{
    if (package.format == 0)
        return unzOpenCurrentFile(package.handle);
    return -1;
}

int FileSystem::PakCloseCurrentFile(PAKid package)
{
    if (package.format == 0)
        return unzCloseCurrentFile(package.handle);
    return -1;
}

int FileSystem::PakCurrentFileRead(void* buffer, uint32_t bytes, PAKid package)
{
    if (package.format == 0)
        return unzReadCurrentFile(package.handle, buffer, bytes);
    return -1;
}
