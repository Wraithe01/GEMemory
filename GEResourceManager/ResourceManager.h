#pragma once

#include "Includes.h"
#include <fstream>
#include <json.hpp>

#include "FileSystem.h"
#include "guiddef.h"
#include "Scene.h"
#include "Resource.h"
#include "AsyncFunctionality.h"
#include "MemoryAlloc.h"

constexpr size_t DEFAULT_MEMORY_LIMIT = 64000000 * 16;

struct HeaderEntry
{
    std::string filename;
    std::string filetype;
    std::string package;
    FilePos     filePos;
};

enum RMAsyncType
{
    RMNone,
    RMLoadChunk,
    RMUnloadChunk
};

struct RMAsyncIn
{
    RMAsyncType type  = RMAsyncType::RMNone;
    Scene*      scene = nullptr;
};

struct RMAsyncOut
{
    int error = false;
};

typedef AsyncRequestHandle<RMAsyncOut> ResourceManagerRequestHandle;

typedef void (*ResourceManagerCallbackFunction)(ResourceManagerRequestHandle request,
                                                void*                        callbackInput);

class ResourceManager : public AsyncFunctionality<RMAsyncIn, RMAsyncOut>
{
private:
    ResourceManager();

    void LoadHeader();

    std::string            GetPackage(const std::string& guid);
    AsyncFileRequestHandle AsyncGetResource(PAKid     package,
                                            FilePos   filePos,
                                            uint8_t*& o_buffer,
                                            int32_t*  o_fileSize);
    void ParseResource(const std::string& guid, uint8_t* buffer, int32_t filesize);

    int RequestLoadScene(const Scene& scene);
    int RequestUnloadScene(const Scene& scene);

    StackAlloc stackAlloc;
    uint8_t* stackStart;
    size_t stackSize = 36;
    std::mutex m_stackLock;

protected:
    virtual void HandleRequest(const RMAsyncIn& requestIN, RMAsyncOut* o_requestOUT) override;


public:
    ~ResourceManager();
    ResourceManager(const ResourceManager& other) = delete;
    void operator=(const ResourceManager& other)  = delete;

    static ResourceManager& GetInstance();

    ResourceManagerRequestHandle LoadScene(Scene& scene);
    ResourceManagerRequestHandle UnloadScene(Scene& scene);
    ResourceManagerRequestHandle LoadScene(Scene&                          scene,
                                           ResourceManagerCallbackFunction callback,
                                           void*                           callbackInput);
    ResourceManagerRequestHandle UnloadScene(Scene&                          scene,
                                             ResourceManagerCallbackFunction callback,
                                             void*                           callbackInput);

    // will return 0 if successful
    int GetRequestError(ResourceManagerRequestHandle request);

    void SetMemoryLimit(size_t limit);
    bool CheckMemoryLimit(size_t fileSize);
    void DumpLoadedResources();
    void AddToQueuedStack(const std::string& guid);
    void UploadQueuedMeshes();

    size_t GetTotalMemoryUsage();

    size_t GetNumOfLoadedRes();

    void LoadedLock();

    void loadedUnlock();

    std::unordered_map<std::string, std::shared_ptr<IMesh>>* GetLoadedMeshes();

    std::unordered_map<std::string, std::shared_ptr<ITexture>>* GetLoadedTextures();

    std::shared_ptr<IMesh> GetMesh(std::string guid);

    std::shared_ptr<ITexture> GetTexture(std::string guid);

private:
    std::unordered_map<std::string, HeaderEntry> m_headerMap;
    std::unordered_map<std::string, std::shared_ptr<IMesh>> m_loadedMeshes;
    std::unordered_map<std::string, std::shared_ptr<ITexture>> m_loadedTextures;
    std::mutex m_loadedLock;
    size_t m_memoryLimit = DEFAULT_MEMORY_LIMIT;
};
