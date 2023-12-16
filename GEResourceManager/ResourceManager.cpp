#include "ResourceManager.h"
#include <set>

// Singleton stuff
ResourceManager::ResourceManager() :
    AsyncFunctionality(RESOURCEMANAGER_ASYNCHTHREADS)
{
    // Init from Resource.h
    InitResourceMap();
}
ResourceManager::~ResourceManager() {}
ResourceManager& ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

ResourceManagerRequestHandle ResourceManager::LoadScene(Scene& scene)
{
    return EnqueueRequest({ RMAsyncType::RMLoadChunk , &scene }, nullptr, nullptr);
}
ResourceManagerRequestHandle ResourceManager::UnloadScene(Scene& scene)
{
    return EnqueueRequest({ RMAsyncType::RMUnloadChunk , &scene }, nullptr, nullptr);
}
ResourceManagerRequestHandle ResourceManager::LoadScene(Scene& scene, ResourceManagerCallbackFunction callback, void* callbackInput)
{
    return EnqueueRequest({ RMAsyncType::RMLoadChunk , &scene }, callback, callbackInput);
}
ResourceManagerRequestHandle ResourceManager::UnloadScene(Scene& scene, ResourceManagerCallbackFunction callback, void* callbackInput)
{
    return EnqueueRequest({ RMAsyncType::RMUnloadChunk , &scene }, callback, callbackInput);
}

int ResourceManager::GetRequestError(ResourceManagerRequestHandle request)
{
    return ReturnDataFromHandle(request)->error;
}

void ResourceManager::ParseResource(const std::string& guid, const packageHandle& packid)
{
    // Get the type of file

    // TODO: ADD TO OFFLINE TOOL
    std::string fext = m_headerMap[guid].filename;
    fext             = fext.substr(fext.find_last_of(".") + 1);
    std::transform(fext.begin(), fext.end(), fext.begin(), ::toupper);

    // Read to buffer
    int32_t  fsize  = 0;
    uint8_t* buffer = nullptr;
    do
    {
        if (PackageSeekFile(packid, m_headerMap[guid].filePos) != UNZ_OK)
            break;
        fsize  = PackageCurrentFileInfo(packid).fileSize;
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
                break;

            case ResourceJPG:
                [[fallthrough]];
            case ResourcePNG:
                res = std::make_shared<Texture>();
                res.get()->LoadResource(static_cast<void*>(buffer), fsize);
                m_loadedData[guid] = res;
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

int ResourceManager::RequestLoadScene(const Scene& scene)
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
        packages[GetPackage(guid)].insert(guid);
    }

    packageHandle phandle = {};
    for (const auto& [key, value] : packages)
    {
        if (value.empty())
            continue;

        phandle = PackageOpen(key.c_str());
        // TODO: Check if package is open
        for (auto& guid : value)
        {
            ParseResource(guid, phandle);
        }
        PackageClose(phandle);
    }
    return 0;
}

int ResourceManager::RequestUnloadScene(const Scene& scene)
{
    for (auto& guid : scene.GetChunk())
    {
        if (m_loadedData.find(guid) != m_loadedData.end())
        {
            int32_t counter = --(*m_loadedData[guid].get());
            if (counter <= 0)
                m_loadedData.erase(guid);
        }
    }
    return 0;
}

void ResourceManager::HandleRequest(const RMAsyncIn& requestIN, RMAsyncOut* o_requestOUT)
{
    switch (requestIN.type)
    {
    case RMAsyncType::RMLoadChunk:
        o_requestOUT->error = RequestLoadScene(*requestIN.scene);
        break;
    case RMAsyncType::RMUnloadChunk:
        o_requestOUT->error = RequestUnloadScene(*requestIN.scene);
        break;
    }
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
