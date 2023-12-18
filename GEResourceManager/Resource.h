#pragma once
#include "Includes.h"
#include "Settings.h"
#include "ufbx.h"

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

private:

protected: // Variables
    std::string m_GUID;
    uint32_t m_refc;
};


class Mesh : public IResource
{
public:
    Mesh() = default;
    virtual ~Mesh() = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override = 0;
    virtual void UnloadResource() override = 0;
};

class FBXMesh sealed : public Mesh
{
public:
    FBXMesh();
    virtual ~FBXMesh() override = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;

private:
    ufbx_scene* m_fbxData;
};

class STLMesh sealed : public Mesh
{
public:
    STLMesh();
    virtual ~STLMesh() override = default;

    virtual bool LoadResource(const uint8_t* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;

private:
    //microstl::MeshReaderHandler handler;
};

class Texture sealed : public IResource
{
public:  // Methods
    Texture();
    ~Texture() = default;

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