#pragma once
#include "Includes.h"
#include "Settings.h"
#include "ufbx.h"

#include <raylib.h>

// Forward declare
namespace microstl
{
    struct MeshReaderHandler;
}

enum AcceptedResourceTypes : uint32_t
{
    ResourceFBX = 0,
    ResourceSTL,
    ResourceJPG,
    ResourcePNG,

    // Keep last
    ResourceCount
};
static std::unordered_map<std::string, AcceptedResourceTypes> g_acceptedTypes;
static void InitResourceMap() {
    g_acceptedTypes["JPG"] = ResourceJPG;
    g_acceptedTypes["PNG"] = ResourcePNG;
    g_acceptedTypes["FBX"] = ResourceFBX;
    g_acceptedTypes["STL"] = ResourceSTL;
}


class IResource
{
public: // Methods
    IResource();
    ~IResource() = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) = 0;
    virtual void UnloadResource()                 = 0;

    // Reference counter functionality
    virtual void SetGUID(const std::string& guid);
    virtual void InitRefcount();

    virtual uint32_t Decrement();
    virtual uint32_t& operator--(); // Pre
    virtual uint32_t  operator--(int); // Post

    virtual uint32_t Increment();
    virtual uint32_t& operator++(); // Pre
    virtual uint32_t  operator++(int); // Post

    virtual uint32_t GetRefcount() const;
    virtual const std::string& GetGUID() const;

    size_t GetMemoryUsage() const;

private:

protected: // Variables
    std::string m_GUID;
    uint32_t m_refc;
    size_t m_memoryUsage;
};


class IMesh : public IResource
{
public:
    IMesh() = default;
    virtual ~IMesh() = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override = 0;
    virtual void UnloadResource() override = 0;
    virtual void ToRayLib() = 0;

protected:
    Mesh* meshes;
};

class FBXMesh sealed : public IMesh
{
public:
    FBXMesh();
    virtual ~FBXMesh() override = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;
    virtual void ToRayLib() override;

private:
    ufbx_scene* m_fbxData;
};

class STLMesh sealed : public IMesh
{
public:
    STLMesh();
    virtual ~STLMesh() override = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;
    virtual void ToRayLib() override;

private:
    microstl::MeshReaderHandler* handler;
};

class ITexture sealed : public IResource
{
public:  // Methods
    ITexture();
    ~ITexture() = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;

private:
private:  // Variables
    uint8_t* m_img;
    struct
    {
        int32_t width;
        int32_t height;
        int32_t channels;
    } m_dim;
};