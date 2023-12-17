#pragma once

#include "PackagingTool.h"
#include "tar.hpp"

class TarGzPackagingTool : public PackagingTool {
public:
    void createPackage(const std::vector<std::string>& assets, const std::string& outputPath) override;
    void gzipTarFile(const std::string& tarFile, const std::string& outputPath);
};
