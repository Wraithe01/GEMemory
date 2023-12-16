#pragma once
// Resource manager includes
//
// This is a file which includes all of the essentials.
// If you find youself needing a include in more than one file, include it here instead of including
// it twice. For simplicity.
//
#pragma once
#include "Settings.h"

#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include <string>

#include "zlib.h"
#define ZLIB_WINAPI
#include "zip.h"
#include "unzip.h"
