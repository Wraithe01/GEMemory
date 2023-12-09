#include "Includes.h"
#include "FileSystem.h"


auto main(void) -> int
{
    CFileSystem FS(4);
    char hello[] = "Hello World!";
    size_t total = 0;
    
    FILEid file = FS.Open("test.txt", "w");
    if (file < 0)
    {
        std::cerr << "Error opening file for append\n";
        return 1;
    }

    for (int i = 0; i < TESTWRITES; i++)
    {
        total += FS.Write(hello, 1, strlen(hello), file);
    }
    if (total < (strlen(hello) * TESTWRITES))
    {
        std::cerr << "Error writing to file\n";
        return 1;
    }

    if (FS.Close(file) != 0)
    {
        std::cerr << "Error closing file\n";
        return 1;
    }
    

    char* readBuf = new char[strlen(hello) * TESTWRITES];
    
    AsyncFileRequestHandle request = FS.AsyncOpenRequest("test.txt", "r", nullptr);

    FS.AsynchRequestWait(request);
    if (!FS.AsyncRequestSucceeded(request))
    {
        std::cerr << "Error async open request failed\n";
        return 1;
    }

    FILEid asyncFile = FS.AsyncGetRequestFileID(request);

    std::cout << "Starting reading async\n";
    request = FS.AsyncReadRequest(readBuf, 1, strlen(hello) * TESTWRITES, asyncFile, nullptr);
    FS.AsynchRequestWait(request);
    if (!FS.AsyncRequestSucceeded(request))
    {
        std::cerr << "Error async read request failed\n";
    }

    total = FS.AsyncGetBytesReadOrWritten(request);
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

