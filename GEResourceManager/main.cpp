#include "Includes.h"
#include "FileSystem.h"

void Callback(AsyncFileRequestHandle request)
{
    std::cout << "Request succeded: " << FileRequestSucceded(request) << std::endl;
    std::cout << "Bytes handled: " << FileRequestBytesReadOrWritten(request) << std::endl;
}

auto main(void) -> int
{
    char hello[] = "Hello World!";
    size_t total = 0;
    
    FILEid file = FileOpen("test.txt", "w");
    if (file < 0)
    {
        std::cerr << "Error opening file for append\n";
        return 1;
    }

    for (int i = 0; i < TESTWRITES; i++)
    {
        total += FileWrite(hello, 1, strlen(hello), file);
    }
    if (total < (strlen(hello) * TESTWRITES))
    {
        std::cerr << "Error writing to file\n";
        return 1;
    }

    if (FileClose(file) != 0)
    {
        std::cerr << "Error closing file\n";
        return 1;
    }
    

    char* readBuf = new char[strlen(hello) * TESTWRITES];
    
    auto request = FileOpenAsync("test.txt", "r", Callback);

    FileRequestWait(request);
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async open request failed\n";
        return 1;
    }

    FILEid asyncFile = FileRequestGetFileID(request);

    std::cout << "Starting reading async\n";
    request = FileReadAsync(readBuf, 1, strlen(hello) * TESTWRITES, asyncFile, Callback);
    FileRequestWait(request);
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async read request failed\n";
    }

    total = FileRequestBytesReadOrWritten(request);
    if (total < (strlen(hello) * TESTWRITES))
    {
        std::cerr << "Error writing to file\n";
        return 1;
    }

    std::cout << "everything works! Guaranteed 100%\n" << "Read: " << total << " bytes\n";

    printf("%.*s\n", 100, readBuf);

    request = FileCloseAsync(asyncFile, Callback);

    FileRequestCallbackWait(request);
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async close request failed\n";
        return 1;
    }

    delete[] readBuf;

    return 0;
}

