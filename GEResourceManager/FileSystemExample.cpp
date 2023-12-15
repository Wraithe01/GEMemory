#include "FileSystemExample.h"
#include "FileSystem.h"
#include "Includes.h"

#define TESTELEMENTS 100000000

struct TestResource
{
    long buffer[TESTELEMENTS];
    size_t elements = TESTELEMENTS;
    uint8_t elementSize = sizeof(long);
};

void CallbackGenerateFile(AsyncFileRequestHandle request, void* input)
{
    // Always check that the request had no errors before doing anything
    if (FileRequestSucceded(request))
    {
        std::cout << "CALLBACK has recieved file and generates buffer\n";
        // the requested file may be reached like so
        FILEid file = FileRequestGetFileID(request);

        TestResource* buf = (TestResource*)input;
        for (size_t i = 0; i < buf->elements; i++)
        {
            buf->buffer[i] = i;
        }

        std::cout << "CALLBACK writing buffer to file\n";
        // Callback runs concurrently with the thread that called the request
        // So using non async file functions could be useful
        size_t written = FileWrite(&(buf->buffer), buf->elementSize, buf->elements, file);

        if (written != buf->elements)
        {
            std::cerr << "ERROR: CALLBACK write error\n";
            return;
        }

        std::cout << "CALLBACK has written " << written * buf->elementSize << " bytes of data\n";

        if (FileClose(file) != 0)
        {
            std::cerr << "ERROR: CALLBACK close file error\n";
            return;
        }

        std::cout << "CALLBACK has closed file and terminated\n";
    }
}
void CallbackReadFile(AsyncFileRequestHandle request, void* input)
{
    std::cout << "Request succeded: " << FileRequestSucceded(request) << std::endl;
    std::cout << "Bytes handled: " << FileRequestBytesReadOrWritten(request) << std::endl;
}

int AsyncFileSystemTest()
{
    TestResource* writeBuf = new TestResource();
    TestResource* readBuf = new TestResource();

    std::cout << "Test: file open request with Writing callback called from MAIN\n";
    // Pushes a request to open a file with a callback function.
    // Use request handle to reach and check the request
    auto request = FileOpenAsync("test", "wb", CallbackGenerateFile, (void*) writeBuf);
    
    // Do work here

    std::cout << "MAIN waits for callback to terminate\n";
    // Synchronization point if needed
    // Blocks until callback terminates
    FileRequestCallbackWait(request);

    std::cout << "MAIN async request to open for reading without callback\n";
    // Pushes a request without callback function
    // Be sure to not lose request handle, this thread must wait for request and extract result
    request = FileOpenAsync("test", "rb", nullptr, nullptr);

    // Do work here

    // this function blocks until the request has completed
    FileRequestWait(request);
    // Don't forget to check for error during request
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async open request failed\n";
        return 1;
    }
    // Retrieve opened file
    FILEid asyncFile = FileRequestGetFileID(request);
    // The file handle may be used for both direct or async calls


    std::cout << "MAIN file opened, starting reading async\n";
    // Async read request without callback function
    request = FileReadAsync(readBuf->buffer, readBuf->elementSize, readBuf->elements, asyncFile, nullptr, nullptr);
    
    // Do work here
    
    // Wait for request
    FileRequestWait(request);
    // Check request
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async read request failed\n";
        return 1;
    }
    // retrieve read bytes
    size_t read = FileRequestBytesReadOrWritten(request);

    std::cout << "MAIN read " << read * readBuf->elementSize << " bytes\n";

    for (int i = 0; i < TESTELEMENTS; i++)
    {
        if (writeBuf->buffer[i] != readBuf->buffer[i])
        {
            std::cerr << "ERROR written and read data mismatch!\n";
            return 1;
        }
    }

    std::cout << "MAIN read data validated\n";

    // async seek request. probbably depends on platform how fast this is. moving a disk reader head could take a long time and justify the async request
    request = FileSeekAsync(asyncFile, 100 * readBuf->elementSize, SeekOrigin::Start, nullptr, nullptr);

    // do work here

    // Wait for request
    FileRequestWait(request);
    // Check request
    if (!FileRequestSucceded(request))
    {
        std::cerr << "Error async seek request failed\n";
        return 1;
    }

    long probe = 0;

    FileRead(&probe, sizeof(long), 1, asyncFile);

    if (probe != 100)
    {
        std::cerr << "Error file seek failed\n";
        return 1;
    }

    std::cout << "MAIN file seek validated\n";

    FileClose(asyncFile);

    std::cout << "MAIN has closed file. Example succeded!\n";
    delete writeBuf;
    delete readBuf;
    return 0;
}
