#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <json.hpp>

#include "FileSystem.h"
#include "guiddef.h"

struct HeaderEntry {
    std::string filename;
    std::string package;
};

class ResourceManager
{
private:
	std::unordered_map<std::string, HeaderEntry> headerMap;

public:
    void LoadHeader();

    std::string GetPackage(const std::string& guid);
};
