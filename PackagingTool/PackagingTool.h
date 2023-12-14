#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "guiddef.h"
#include "combaseapi.h"

class PackagingTool {
public:
    virtual ~PackagingTool() {}

    virtual void createPackage(const std::vector<std::string>& assets, const std::string& outputPath) = 0;

    static void generateGUID(GUID& guid);

    static void createHeaderFile(const std::vector<std::string>& assets, const std::string& headerFilePath,
        const std::string& packageName);
};