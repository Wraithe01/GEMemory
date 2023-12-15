#pragma once
#include "Includes.h"


class Resource
{
public: // Methods
    Resource() = default;
    ~Resource() = default;

    virtual bool LoadResource(const std::string& filepath) = 0;
    virtual void UnloadResource()                   = 0;

    // Reference counter functionality

    virtual uint32_t Decrement();
    virtual uint32_t& operator--(); // Pre
    virtual uint32_t  operator--(int); // Post

    virtual uint32_t Increment();
    virtual uint32_t& operator++(); // Pre
    virtual uint32_t  operator++(int); // Post

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

    virtual bool LoadResource(const std::string& filepath) override;
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

    virtual bool LoadResource(const std::string& filepath) override;
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
