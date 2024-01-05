#pragma once

#include "PackagingTool.h"

class ScenePackaging {
public:
    void createPackage(std::vector<std::string>& directories, const std::string& inputPath, const std::string& outputPath);
};