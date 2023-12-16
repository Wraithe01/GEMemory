#pragma once
#include "Includes.h"
#include "ufbx.h"

enum AcceptedResourceTypes : uint32_t
{
    ResourceFBX = 0,
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
}


class IResource
{
public: // Methods
    IResource();
    ~IResource() = default;

    virtual bool LoadResource(const void* buffer, int32_t buffSize) = 0;
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


class Mesh sealed : public IResource
{
public: // Methods
    Mesh();
    ~Mesh() = default;

    virtual bool LoadResource(const void* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;

private:
private: // Variables
    ufbx_scene* m_data;
};

class Texture sealed : public IResource
{
public:  // Methods
    Texture();
    ~Texture() = default;

    virtual bool LoadResource(const void* buffer, int32_t buffSize) override;
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
