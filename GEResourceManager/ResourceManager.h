#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <json.hpp>

#include "FileSystem.h"
#include "guiddef.h"

struct HeaderEntry
{
    std::string filename;
    std::string package;
};

class ResourceManager sealed
{
private:
    ResourceManager();

    void LoadHeader();

    std::string GetPackage(const std::string& guid);

public:
    ~ResourceManager();
    ResourceManager(const ResourceManager& obj) = delete;
    static ResourceManager* GetInstance();

private:
    static ResourceManager* m_instance;

    std::unordered_map<std::string, HeaderEntry> m_headerMap;
};
