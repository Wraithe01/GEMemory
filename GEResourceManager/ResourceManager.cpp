#include "ResourceManager.h"
#include <set>

// Singleton stuff
ResourceManager::ResourceManager()
{
    // Init from Resource.h
    InitResourceMap();

    // Load json header into memory
    LoadHeader();
}
ResourceManager::~ResourceManager() {}
ResourceManager& ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}


void ResourceManager::UnloadScene(const Scene& scene)
{
    for (auto& guid : scene.GetChunk())
    {
        if (m_loadedData.find(guid) != m_loadedData.end())
        {
            int32_t counter = --(*m_loadedData[guid].get());
            if (counter <= 0)
            {
                // TODO : decrease memorysize!
                m_loadedData.erase(guid);
            }
        }
    }
}
void ResourceManager::LoadScene(const Scene& scene)
{
    std::map<std::string, std::set<std::string>> packages;
    for (auto& guid : scene.GetChunk())
    {
        const auto& it = m_loadedData.find(guid);
        if (it != m_loadedData.end())
        {
            ++(*m_loadedData[guid].get());
            continue;
        }
        std::string package = GetPackage(guid);

        if (strcmp(package.c_str(), "") == 0)
        {
            std::cerr << "Could not find package for guid " << guid << std::endl;
            continue;
        }
        packages[package].insert(guid);
    }

    // TODO: Make async
    packageHandle phandle = {};
    for (const auto& [key, value] : packages)
    {
        if (value.empty())
            continue;

        phandle = PackageOpen(key.c_str());

        if (phandle.handle == nullptr)
        {
            std::cerr << "Could not open package " << key << std::endl;
            continue;
        }

        for (auto& guid : value)
        {
            ParseResource(guid, phandle);
        }
        PackageClose(phandle);
    }
}
void ResourceManager::ParseResource(const std::string& guid, const packageHandle& packid)
{
    // Get the type of file
    std::string fext = m_headerMap[guid].filetype;
    std::transform(fext.begin(), fext.end(), fext.begin(), ::toupper);

    // Read to buffer
    int32_t  fsize = 0;
    uint8_t* buffer = nullptr;
    do
    {
        if (PackageSeekFile(packid, m_headerMap[guid].filePos) != UNZ_OK)
            break;
        fsize = PackageCurrentFileInfo(packid).fileSize;

        // Memory limit check
        m_memoryUsage += fsize;
        if (CheckMemoryLimit()) {
            m_memoryUsage -= fsize;
            return;
        };

        buffer = static_cast<uint8_t*>(std::malloc(fsize * sizeof(uint8_t)));
        if (PackageCurrentFileOpen(packid) != UNZ_OK)
            break;

        if (PackageCurrentFileRead(buffer, fsize, packid) != fsize)
            break;
        PackageCurrentFileClose(packid);

        std::shared_ptr<IResource> res;
        switch (g_acceptedTypes[fext])
        {
        case ResourceFBX:
            res = std::make_shared<Mesh>();
            res.get()->LoadResource(static_cast<void*>(buffer), fsize);
            m_loadedData[guid] = res;
            printf("Loaded FBX!\n");
            break;

        case ResourceJPG:
            [[fallthrough]];
        case ResourcePNG:
            res = std::make_shared<Texture>();
            res.get()->LoadResource(static_cast<void*>(buffer), fsize);
            m_loadedData[guid] = res;
            printf("Loaded PNG/JPG!\n");
            break;

        default:
            std::cerr << "Filetype " << fext << " is not recognized." << std::endl;
            break;
        }
        m_loadedData[guid]->InitRefcount();
    } while (0);
    if (buffer != nullptr)
        std::free(buffer);
}


void ResourceManager::LoadHeader()
{
    // Read the JSON file
    std::ifstream file("../PackageFolder/header.json");
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
        const std::string& guid = entry.key();
        const std::string& filename = entry.value()["filename"];
        const std::string& filetype = entry.value()["filetype"];
        const std::string& package = entry.value()["package"];
        const uLong          offset = entry.value()["offset"];
        const uLong          fileNumber = entry.value()["filenumber"];
        const unz_file_pos_s filePos{ offset, fileNumber };

        HeaderEntry entryData{ filename, filetype, package, filePos };
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

void ResourceManager::SetMemoryLimit(size_t limit) {
    m_memoryLimit = limit;
}

bool ResourceManager::CheckMemoryLimit() const {
    if (m_memoryLimit > 0 && m_memoryUsage > m_memoryLimit) {
        std::cerr << "Warning: Memory limit exceeded! Total memory usage: "
            << m_memoryUsage << " bytes! Skipping to add resource\n";
        return true;
    }
    return false;
}

size_t ResourceManager::GetMemoryUsage() {
    return m_memoryUsage;
}

size_t ResourceManager::GetNumOfLoadedRes()
{
    return m_loadedData.size();
}
