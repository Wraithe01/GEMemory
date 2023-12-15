#include "Settings.h"
#include "Resource.h"
#include "ufbx.h"
#include "stb_image.h"

void Resource::InitRefcount() { m_refc = 1; }

uint32_t  Resource::Decrement() { return --m_refc; }
uint32_t& Resource::operator--() { return --m_refc; }
uint32_t  Resource::operator--(int) { return m_refc--; }

uint32_t  Resource::Increment() { return ++m_refc; }
uint32_t& Resource::operator++() { return ++m_refc; }
uint32_t  Resource::operator++(int) { return m_refc++; }

uint32_t Resource::GetRefcount() const { return m_refc; }


bool Mesh::LoadResource(const std::string& filepath)
{
    ufbx_error err;
    m_data = ufbx_load_file(filepath.c_str(), NULL, &err);
    if (!m_data)
    {
        std::cerr << "Failed to load mesh (fbx) in path \"" << filepath
                  << "\", with error: " << err.description.data << std::endl;
        return false;
    }
    return true;
}
void Mesh::UnloadResource()
{
    ufbx_free_scene(m_data);
    m_data = nullptr;
}


bool Texture::LoadResource(const std::string& filepath)
{
    m_img = stbi_load(
        filepath.c_str(), &m_dim.width, &m_dim.height, &m_dim.channels, img_max_channels);
    if (!m_img)
    {
        std::cerr << "Failed to load texture (png) in path \"" << filepath << "\"." << std::endl;
        return false;
    }
    return true;
}
void Texture::UnloadResource()
{
    delete m_img;
    m_img = nullptr;
}
