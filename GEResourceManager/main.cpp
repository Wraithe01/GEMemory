#include "Includes.h"
#include "FileSystem.h"
#include "FileSystemExample.h"
#include "ResourceTest.h"

auto main(void) -> int
{
    ResourceTest tester;
    tester.Validate();
    //tester.PerformanceBenchmark();

    int err = 0;
    err = AsyncFileSystemTest();
    return err;
}