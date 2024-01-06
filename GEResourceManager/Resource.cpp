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
    // Create the meshes array
    size_t size = m_fbxData->meshes.count * sizeof(Mesh);
    this->meshes = (Mesh*)malloc(size);
    memset(this->meshes, 0, size);

    for (size_t meshIndex = 0; meshIndex < m_fbxData->meshes.count; meshIndex++)
    {
        Mesh& raylibMesh = this->meshes[meshIndex];
        ufbx_mesh* fbxMesh = m_fbxData->meshes.data[meshIndex];

        size_t verticesCount = fbxMesh->vertices.count;
        size_t normals = fbxMesh->vertex_normal.values.count;
        size_t uvs = fbxMesh->vertex_uv.values.count;
     
        raylibMesh.vertexCount = verticesCount;
       
        // Allocate memory for vertex positions, normals, and texture coordinates
        raylibMesh.vertices = (float*)MemAlloc(verticesCount * 3 * sizeof(float)); // 3 coordinates each (x, y, z)
        raylibMesh.normals = (float*)MemAlloc(normals * 3 * sizeof(float)); // 3 coordinates each (x, y, z)
        raylibMesh.texcoords = (float*)MemAlloc(uvs * 2 * sizeof(float)); // 2 coordinates each (x, y)

        // VERTEX
        for (int i = 0; i < verticesCount; i++) {
           raylibMesh.vertices[i * 3] = (float)fbxMesh->vertices[i].x;
           raylibMesh.vertices[i * 3 + 1] = (float)fbxMesh->vertices[i].y;
           raylibMesh.vertices[i * 3 + 2] = (float)fbxMesh->vertices[i].z;
        }

        // NORMALS
        for (int i = 0; i < normals; i++) {
            raylibMesh.normals[i * 3] = (float)fbxMesh->vertex_normal[i].x;
            raylibMesh.normals[i * 3 + 1] = (float)fbxMesh->vertex_normal[i].y;
            raylibMesh.normals[i * 3 + 2] = (float)fbxMesh->vertex_normal[i].z;
        }

        // UVS
        for (int i = 0; i < uvs; i++)
        {
            raylibMesh.texcoords[i * 2] = fbxMesh->vertex_uv[i].x;
            raylibMesh.texcoords[i * 2 + 1] = fbxMesh->vertex_uv[i].y;
        }

        // FACES
        int totalTriangles = 0;
        // First pass: count the total number of triangles needed
        for (int i = 0; i < fbxMesh->faces.count; i++) {
            ufbx_face face = fbxMesh->faces.data[i];
            if (face.num_indices >= 3) {
                totalTriangles += face.num_indices - 2;
            }
        }

        // Allocate memory for the triangles
        raylibMesh.indices = (unsigned short*)malloc(totalTriangles * 3 * sizeof(unsigned short));
        raylibMesh.triangleCount = totalTriangles;

        // Second pass: triangulate faces
        int k = 0;
        for (int i = 0; i < fbxMesh->faces.count; i++) {
            ufbx_face face = fbxMesh->faces.data[i];
            if (face.num_indices < 3) continue; // Skip invalid faces

            // Fan triangulation for convex polygons
            for (int j = 2; j < face.num_indices; j++) {
                raylibMesh.indices[k++] = fbxMesh->vertex_indices.data[face.index_begin];
                raylibMesh.indices[k++] = fbxMesh->vertex_indices.data[face.index_begin + j - 1];
                raylibMesh.indices[k++] = fbxMesh->vertex_indices.data[face.index_begin + j];
            }
        }

        // EDIT: Does not work here... :(
        // UploadMesh(&raylibMesh, false);

        //this->meshes[meshIndex] = raylibMesh;
    }
    meshCount = m_fbxData->meshes.count;
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

void STLMesh::ToRayLib()
{ 
    this->meshCount = 1;
    size_t size = this->meshCount * sizeof(Mesh);
    this->meshes = (Mesh*)malloc(size);
    Mesh raylibMesh = { 0 };

    // Assuming each facet is a triangle
    size_t numFacets = handler->mesh.facets.size();
    raylibMesh.vertexCount = numFacets * 3; // 3 vertices per triangle
    raylibMesh.triangleCount = numFacets;

    raylibMesh.vertices = (float*)malloc(raylibMesh.vertexCount * 3 * sizeof(float)); // 3 floats per vertex
    raylibMesh.normals = (float*)malloc(raylibMesh.vertexCount * 3 * sizeof(float)); // 3 floats per normal

    for (size_t i = 0; i < numFacets; i++) {
        const auto& facet = handler->mesh.facets[i];

        // Set vertex positions and normals for each vertex in the facet
        const microstl::Vertex* vertices[3] = { &facet.v1, &facet.v2, &facet.v3 };
        for (int j = 0; j < 3; j++) {
            raylibMesh.vertices[(i * 3 + j) * 3 + 0] = vertices[j]->x;
            raylibMesh.vertices[(i * 3 + j) * 3 + 1] = vertices[j]->y;
            raylibMesh.vertices[(i * 3 + j) * 3 + 2] = vertices[j]->z;

            raylibMesh.normals[(i * 3 + j) * 3 + 0] = facet.n.x;
            raylibMesh.normals[(i * 3 + j) * 3 + 1] = facet.n.y;
            raylibMesh.normals[(i * 3 + j) * 3 + 2] = facet.n.z;
        }
    }
    // The mesh is not indexed; indices are not needed
    raylibMesh.indices = nullptr;
    this->meshes[0] = raylibMesh;
}


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

Mesh* IMesh::GetMeshes()
{
    return meshes;
}

size_t IMesh::GetMeshCount()
{
    return meshCount;
}
