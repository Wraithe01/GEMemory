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
std::unordered_map<std::string, AcceptedResourceTypes> g_acceptedTypes;
void InitResourceMap() {
    g_acceptedTypes["JPG"] = ResourceJPG;
    g_acceptedTypes["PNG"] = ResourcePNG;
    g_acceptedTypes["FBX"] = ResourceFBX;
}


class Resource
{
public: // Methods
    Resource() = default;
    ~Resource() = default;

    virtual bool LoadResource(const void* buffer, int32_t buffSize) = 0;
    virtual void UnloadResource()                 = 0;

    // Reference counter functionality

    virtual void InitRefcount();

    virtual uint32_t Decrement();
    virtual uint32_t& operator--(); // Pre
    virtual uint32_t  operator--(int); // Post

    virtual uint32_t Increment();
    virtual uint32_t& operator++(); // Pre
    virtual uint32_t  operator++(int); // Post

    virtual uint32_t GetRefcount() const;

private:

protected: // Variables
    uint32_t m_GUID;
    uint32_t m_refc;
};


class Mesh sealed : public Resource
{
public: // Methods
    Mesh() = default;
    ~Mesh() = default;

    virtual bool LoadResource(const void* buffer, int32_t buffSize) override;
    virtual void UnloadResource() override;

private:
private: // Variables
    ufbx_scene* m_data;
};

class Texture sealed : public Resource
{
public:  // Methods
    Texture() = default;
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
