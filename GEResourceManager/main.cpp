#include "Includes.h"
#include "FileSystem.h"
#include "FileSystemExample.h"


auto main(void) -> int
{
    int err = 0;
    err = AsyncFileSystemTest();
    return err;
}

