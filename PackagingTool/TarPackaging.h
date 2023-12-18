#pragma once

#include "PackagingTool.h"
#include "microtar.h"
#include "zlib.h"

class TarPackagingTool : public PackagingTool {
public:
    void createPackage(std::vector<std::string>& assets, const std::string& outputPathh) override;
};
