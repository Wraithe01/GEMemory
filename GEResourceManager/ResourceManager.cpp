#include "ResourceManager.h"
#include <set>

// Singleton stuff
ResourceManager::ResourceManager()
{
    m_instance = nullptr;
    // Init from Resource.h
    InitResourceMap();
}
ResourceManager::~ResourceManager()
{
    if (m_instance)
        delete m_instance;
}
ResourceManager* ResourceManager::GetInstance()
{
    if (!m_instance)
        m_instance = new ResourceManager();
    return m_instance;
}

void ResourceManager::LoadScene(const Scene& scene)
{
    std::map<std::string, std::set<std::string>> packages;
    for (auto& guid : scene.GetChunk())
    {
        const auto& it = m_loadedData.find(guid);
        if (it != m_loadedData.end())
        {
            ++m_loadedData[guid];
            continue;
        }
        packages[GetPackage(guid)].insert(guid);
    }

    for (const auto& [key, value] : packages)
    {
        for (auto& guid : value)
        {
            ParseResource(guid, key);
        }
    }
}
void ResourceManager::ParseResource(const std::string& guid, const std::string& package)
{
    // Get the type of file

    // TODO: ADD TO OFFLINE TOOL
    // std::string fext;
    // std::transform(fext.begin(), fext.end(), fext.begin(), ::toupper);

    // Read to buffer


    // Resource* res = nullptr;
    // switch (g_acceptedTypes[fext])
    //{
    //     case ResourceFBX:
    //         dynamic_cast<Mesh*>(res)->LoadResource();
    //         m_loadedData[guid] = res;
    //         break;

    //    case ResourceJPG:
    //    case ResourcePNG:
    //        dynamic_cast<Mesh*>(res)->LoadResource();
    //        m_loadedData[guid] = res;
    //        break;
    //}
    // m_loadedData[guid]->InitRefcount();
}

void ResourceManager::LoadHeader()
{
    // Read the JSON file
    std::ifstream file("../Packages/header.json");
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON file\n";
        return;
    }
    // Parse JSON
    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return;
    }

    for (const auto& entry : jsonData.items())
    {
        const std::string&   guid       = entry.key();
        const std::string&   filename   = entry.value()["filename"];
        const std::string&   package    = entry.value()["package"];
        const uLong          offset     = entry.value()["filepos"];
        const uLong          fileNumber = entry.value()["filenumber"];
        const unz_file_pos_s filePos{ offset, fileNumber };

        HeaderEntry entryData{ filename, package, filePos };
        m_headerMap[guid] = entryData;
    }
}
std::string ResourceManager::GetPackage(const std::string& guid)
{
    const auto& it = m_headerMap.find(guid);

    if (it != m_headerMap.end())
    {
        return it->second.package;
    }
    return "";
}
