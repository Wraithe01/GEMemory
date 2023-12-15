#include "Settings.h"
#include "Resource.h"
#include "stb_image.h"

void Resource::InitRefcount() { m_refc = 1; }

uint32_t  Resource::Decrement() { return --m_refc; }
uint32_t& Resource::operator--() { return --m_refc; }
uint32_t  Resource::operator--(int) { return m_refc--; }

uint32_t  Resource::Increment() { return ++m_refc; }
uint32_t& Resource::operator++() { return ++m_refc; }
uint32_t  Resource::operator++(int) { return m_refc++; }

uint32_t Resource::GetRefcount() const { return m_refc; }


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
        std::cerr << "Failed to load texture (png)." << std::endl;
        return false;
    }
    return true;
}
void Texture::UnloadResource()
{
    delete m_img;
    m_img = nullptr;
}