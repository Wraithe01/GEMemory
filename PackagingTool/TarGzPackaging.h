#pragma once

#include "PackagingTool.h"

class TarGzPackagingTool : public PackagingTool {
public:
    void createPackage(const std::vector<std::string>& assets, const std::string& outputPath) override;
};
