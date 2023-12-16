#pragma once

#include "Includes.h"
#include <fstream>
#include <json.hpp>

#include "FileSystem.h"
#include "guiddef.h"
#include "Scene.h"
#include "Resource.h"

constexpr size_t DEFAULT_MEMORY_LIMIT = 64000000;

struct HeaderEntry
{
    std::string    filename;
    std::string    filetype;
    std::string    package;
    unz_file_pos_s filePos;
};

class ResourceManager sealed
{
private:
    ResourceManager();

    void LoadHeader();

    std::string GetPackage(const std::string& guid);
    void        ParseResource(const std::string& guid, const packageHandle& packid);


public:
    ~ResourceManager();
    ResourceManager(const ResourceManager& other) = delete;
    void operator=(const ResourceManager& other) = delete;

    static ResourceManager& GetInstance();

    void LoadScene(const Scene& scene);
    void UnloadScene(const Scene& scene);

    void SetMemoryLimit(size_t limit);
    bool CheckMemoryLimit() const;

    size_t GetMemoryUsage();

    size_t GetNumOfLoadedRes();

private:
    std::unordered_map<std::string, HeaderEntry>                m_headerMap;
    std::unordered_map<std::string, std::shared_ptr<IResource>> m_loadedData;
    size_t m_memoryUsage;
    size_t m_memoryLimit = DEFAULT_MEMORY_LIMIT;
};