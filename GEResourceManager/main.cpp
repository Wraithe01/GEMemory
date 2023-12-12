#include "Includes.h"
#include "FileSystem.h"

void Callback(AsyncFileRequestHandle request)
{
    std::cout << "Request succeded: " << CFileSystem::Instance()->AsyncRequestSucceeded(request) << std::endl;
    std::cout << "Bytes handled: " << CFileSystem::Instance()->AsyncGetBytesReadOrWritten(request) << std::endl;
}

auto main(void) -> int
{
    char hello[] = "Hello World!";
    size_t total = 0;
    
    FILEid file = CFileSystem::Instance()->Open("test.txt", "w");
    if (file < 0)
    {
        std::cerr << "Error opening file for append\n";
        return 1;
    }

    for (int i = 0; i < TESTWRITES; i++)
    {
        total += CFileSystem::Instance()->Write(hello, 1, strlen(hello), file);
    }
    if (total < (strlen(hello) * TESTWRITES))
    {
        std::cerr << "Error writing to file\n";
        return 1;
    }

    if (CFileSystem::Instance()->Close(file) != 0)
    {
        std::cerr << "Error closing file\n";
        return 1;
    }
    

    char* readBuf = new char[strlen(hello) * TESTWRITES];
    
    AsyncFileRequestHandle request = CFileSystem::Instance()->AsyncOpenRequest("test.txt", "r", Callback);

    CFileSystem::Instance()->AsynchRequestWait(request);
    if (!CFileSystem::Instance()->AsyncRequestSucceeded(request))
    {
        std::cerr << "Error async open request failed\n";
        return 1;
    }

    FILEid asyncFile = CFileSystem::Instance()->AsyncGetRequestFileID(request);

    std::cout << "Starting reading async\n";
    request = CFileSystem::Instance()->AsyncReadRequest(readBuf, 1, strlen(hello) * TESTWRITES, asyncFile, Callback);
    CFileSystem::Instance()->AsynchRequestWait(request);
    if (!CFileSystem::Instance()->AsyncRequestSucceeded(request))
    {
        std::cerr << "Error async read request failed\n";
    }

    total = CFileSystem::Instance()->AsyncGetBytesReadOrWritten(request);
    if (total < (strlen(hello) * TESTWRITES))
    {
        std::cerr << "Error writing to file\n";
        return 1;
    }

    std::cout << "everything works! Guaranteed 100%\n" << "Read: " << total << " bytes\n";

    printf("%.*s\n", 100, readBuf);

    delete[] readBuf;

    return 0;
}

