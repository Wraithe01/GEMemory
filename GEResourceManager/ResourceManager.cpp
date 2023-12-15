#include "ResourceManager.h"

// Singleton stuff
ResourceManager::ResourceManager() { m_instance = nullptr; }
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
        const std::string& guid     = entry.key();
        const std::string& filename = entry.value()["filename"];
        const std::string& package  = entry.value()["package"];

        HeaderEntry entryData{ filename, package };
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
