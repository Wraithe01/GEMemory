#pragma once

#include "PackagingTool.h"
#include "zlib.h"
#define ZLIB_WINAPI
#include "zip.h"
#include "unzip.h"
#include <fstream>

class ZipPackagingTool : public PackagingTool {
public:
    virtual void createPackage(std::vector<std::string>& assets, const std::string& outputPath) override;
};