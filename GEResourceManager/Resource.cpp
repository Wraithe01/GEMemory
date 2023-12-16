#include "Settings.h"
#include "Resource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

IResource::IResource()
: m_refc(0)
{
}

void IResource::SetGUID(const std::string& guid) { m_GUID = guid; }
void IResource::InitRefcount() { m_refc = 1; }

uint32_t  IResource::Decrement() { return --m_refc; }
uint32_t& IResource::operator--() { return --m_refc; }
uint32_t  IResource::operator--(int) { return m_refc--; }

uint32_t  IResource::Increment() { return ++m_refc; }
uint32_t& IResource::operator++() { return ++m_refc; }
uint32_t  IResource::operator++(int) { return m_refc++; }

uint32_t IResource::GetRefcount() const { return m_refc; }

const std::string& IResource::GetGUID() const { return m_GUID; }


Mesh::Mesh()
: m_data(nullptr)
{
}

bool Mesh::LoadResource(const void* buffer, int32_t buffSize)
{
    ufbx_error err;
    ufbx_load_memory(buffer, buffSize, NULL, &err);
    if (!m_data)
    {
        std::cerr << "Failed to load mesh (fbx), with error: " << err.description.data << std::endl;
        return false;
    }
    return true;
}
void Mesh::UnloadResource()
{
    ufbx_free_scene(m_data);
    m_data = nullptr;
}


Texture::Texture()
: m_img(nullptr)
{
    m_dim = {};
}

bool Texture::LoadResource(const void* buffer, int32_t buffSize)
{
    m_img = stbi_load_from_memory((stbi_uc*) buffer,
                                  buffSize,
                                  &m_dim.width,
                                  &m_dim.height,
                                  &m_dim.channels,
                                  img_max_channels);
    if (!m_img)
    {
        std::cerr << "Failed to load texture (png/jpg)." << std::endl;
        return false;
    }
    return true;
}
void Texture::UnloadResource()
{
    stbi_image_free(m_img);
    m_img = nullptr;
}
