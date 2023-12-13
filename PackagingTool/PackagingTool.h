#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "guiddef.h"
#include "combaseapi.h"

class PackagingTool {
public:
    virtual ~PackagingTool() {}

    virtual void createPackage(const std::vector<std::string>& assets, const std::string& outputPath) = 0;

    static void generateGUID(GUID& guid);

    static void createMappingFile(const std::vector<std::string>& assets, const std::string& mappingFilePath);
};