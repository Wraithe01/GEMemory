#include "ResourceManager.h"
#include <set>

// Singleton stuff
ResourceManager::ResourceManager() :
    AsyncFunctionality(RESOURCEMANAGER_ASYNCHTHREADS)
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
        std::string package = GetPackage(guid);

        if (strcmp(package.c_str(), "") == 0)
        {
            std::cerr << "Could not find package for guid " << guid << std::endl;
            continue;
        }
        packages[package].insert(guid);
    }

    packageHandle phandle = {};
    for (const auto& [key, value] : packages)
    {
        if (value.empty())
            continue;

        phandle = PackageOpen(key.c_str());
        if (!PackageWasOpened(phandle))
        {
            std::cerr << "Could not open package " << key << std::endl;
            continue;
        }

        AsyncFileRequestHandle fileRequest = nullptr;
        uint8_t* currentBuf = nullptr;
        int32_t currentBufSize = 0;
        uint8_t* nextBuf = nullptr;
        int32_t nextBufSize = 0;

        int i = 0;
        for(auto it = value.begin(); it != value.end(); it++)
        {
            i++;
            if (it == value.begin())
            {
                fileRequest = AsyncGetResource(phandle, m_headerMap[*it].filePos, nextBuf, &nextBufSize);
            }
            FileRequestCallbackWait(fileRequest);
            currentBuf = nextBuf;
            currentBufSize = nextBufSize;
            if (i < value.size())
            {
                it++;
                fileRequest = AsyncGetResource(phandle, m_headerMap[*it].filePos, nextBuf, &nextBufSize);
                it--;
            }
            if (currentBufSize < 0)
            {
                std::cerr << "ERROR: reading file content. GUID: " << *it << std::endl;
            }
            else
            {
                ParseResource(*it, currentBuf, currentBufSize);
            }
        }
        PackageClose(phandle);
    }
    return 0;
}

struct FileBufCallback
{
    PAKid packid;
    uint8_t* buffer;
    int32_t* bufSize;
};

void CallbackPakFile(AsyncFileRequestHandle request, void* input)
{
    if (!FileRequestSucceded(request))
    {
        std::cerr << "ERROR: package current file could not be opened\n";
        return;
    }
    FileBufCallback* buffer = (FileBufCallback*)input;

    int32_t requested = *(buffer->bufSize);

    *(buffer->bufSize) = PackageCurrentFileRead(buffer->buffer, *(buffer->bufSize), buffer->packid);

    if (*(buffer->bufSize) != requested)
    {
        std::cerr << "ERROR: could not read whole file\n";
        delete buffer->buffer;
        *(buffer->bufSize) = -1;
    }

    PackageCurrentFileClose(buffer->packid);

    delete input;
}

AsyncFileRequestHandle ResourceManager::AsyncGetResource(PAKid package, FilePos filePos, uint8_t*& o_buffer, int32_t* o_fileSize)
{
    // Read to buffer
    *o_fileSize = 0;
    o_buffer = nullptr;

    AsyncFileRequestHandle fileRequest = nullptr;

    if (PackageSeekFile(package, filePos) != UNZ_OK)
    {
        std::cerr << "ERROR: Seek file failed\n";
        return nullptr;
    }

    *o_fileSize = PackageCurrentFileInfo(package).fileSize;
    o_buffer = static_cast<uint8_t*>(std::malloc((*o_fileSize) * sizeof(uint8_t)));
    
    //opens async request for opening current file. File content will be read in callback
    fileRequest = PackageCurrentFileOpenAsync(package, CallbackPakFile, new FileBufCallback({ package, o_buffer, o_fileSize }));

    return fileRequest;
}

void ResourceManager::ParseResource(const std::string& guid, uint8_t* buffer, int32_t filesize)
{
    // Get the type of file
    std::string fext = m_headerMap[guid].filetype;
    std::transform(fext.begin(), fext.end(), fext.begin(), ::toupper);

    std::shared_ptr<IResource> res;
    switch (g_acceptedTypes[fext])
    {
        case ResourceFBX:
            res = std::make_shared<Mesh>();
            res.get()->LoadResource(static_cast<void*>(buffer), filesize);
            m_loadedData[guid] = res;
            printf("Loaded FBX!\n");
            break;

        case ResourceJPG:
            [[fallthrough]];
        case ResourcePNG:
            res = std::make_shared<Texture>();
            res.get()->LoadResource(static_cast<void*>(buffer), filesize);
            m_loadedData[guid] = res;
            printf("Loaded PNG/JPG!\n");
            break;

        default:
            std::cerr << "Filetype " << fext << " is not recognized." << std::endl;
            break;
    }
    m_loadedData[guid]->InitRefcount();

    if (buffer != nullptr)
        delete buffer;
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
