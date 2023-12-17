#pragma once

#include "PackagingTool.h"
#include "tar.hpp"

class TarPackagingTool : public PackagingTool {
public:
    void createPackage(std::vector<std::string>& assets, const std::string& outputPathh) override;
};
