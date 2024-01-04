#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"

#include "microstl.h"

IResource::IResource()
: m_refc(0)
, m_memoryUsage(0)
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

size_t IResource::GetMemoryUsage() const { return m_memoryUsage; }

FBXMesh::FBXMesh()
: m_fbxData(nullptr)
{
}

void FBXMesh::ToRayLib()
{
    size_t size = m_fbxData->meshes.count * sizeof(Mesh);
    meshes      = (Mesh*) malloc(size);
    memset(meshes, 0, size);

    for (size_t meshIndex = 0; meshIndex < m_fbxData->meshes.count; meshIndex++)
    {
        Mesh raylibMesh = { 0 };

        ufbx_mesh* fbxMesh       = m_fbxData->meshes.data[meshIndex];
        raylibMesh.vertexCount   = fbxMesh->num_vertices;
        raylibMesh.triangleCount = fbxMesh->num_triangles;

        // Allocate memory for vertex positions, normals, and texture coordinates
        raylibMesh.vertices  = (float*) malloc(raylibMesh.vertexCount * 3 * sizeof(float));
        raylibMesh.normals   = (float*) malloc(raylibMesh.vertexCount * 3 * sizeof(float));
        raylibMesh.texcoords = (float*) malloc(raylibMesh.vertexCount * 2 * sizeof(float));

        // Copy vertex data from FBX to Raylib
        for (size_t i = 0; i < raylibMesh.vertexCount; i++)
        {
            ufbx_vec3 fbxVertex   = fbxMesh->vertices[i];
            ufbx_vec3 fbxNormal   = fbxMesh->vertex_normal[i];  // Correct normal type?
            ufbx_vec2 fbxTexCoord = fbxMesh->vertex_uv[i];      // Correct uv type?

            raylibMesh.vertices[i * 3]     = fbxVertex.x;
            raylibMesh.vertices[i * 3 + 1] = fbxVertex.y;
            raylibMesh.vertices[i * 3 + 2] = fbxVertex.z;

            raylibMesh.normals[i * 3]     = fbxNormal.x;
            raylibMesh.normals[i * 3 + 1] = fbxNormal.y;
            raylibMesh.normals[i * 3 + 2] = fbxNormal.z;

            raylibMesh.texcoords[i * 2]     = fbxTexCoord.x;
            raylibMesh.texcoords[i * 2 + 1] = fbxTexCoord.y;
        }
        meshes[meshIndex] = raylibMesh;
    }
}

bool FBXMesh::LoadResource(const uint8_t* buffer, int32_t buffSize)
{
    ufbx_error err;
    m_fbxData = ufbx_load_memory(buffer, buffSize, NULL, &err);
    if (!m_fbxData)
    {
        std::cerr << "Failed to load FBX mesh, with error: " << err.description.data << std::endl;
        return false;
    }
    m_memoryUsage = buffSize;
    return true;
}

void FBXMesh::UnloadResource()
{
    ufbx_free_scene(m_fbxData);
    m_fbxData     = nullptr;
    m_memoryUsage = 0;
}

STLMesh::STLMesh() { handler = new microstl::MeshReaderHandler; }

bool STLMesh::LoadResource(const uint8_t* buffer, int32_t buffSize)
{
    microstl::Result result = microstl::Reader::readStlBuffer(
        reinterpret_cast<const char*>(buffer), buffSize, *handler);

    if (result != microstl::Result::Success)
    {
        std::cerr << "Failed to load STL model: " << microstl::getResultString(result) << std::endl;
        return false;
    }
    m_memoryUsage = buffSize;
    return true;
}

void STLMesh::UnloadResource()
{
    m_memoryUsage = 0;
    if (handler != nullptr)
        delete handler;
}

void STLMesh::ToRayLib() { std::cerr << "STL ToRayLib NYI.\n"; }


ITexture::ITexture()
: m_img(nullptr)
{
    m_dim = {};
}

bool ITexture::LoadResource(const uint8_t* buffer, int32_t buffSize)
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
    m_memoryUsage = buffSize;
    return true;
}
void ITexture::UnloadResource()
{
    stbi_image_free(m_img);
    m_img         = nullptr;
    m_memoryUsage = 0;
}
