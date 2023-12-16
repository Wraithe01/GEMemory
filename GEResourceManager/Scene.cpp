#include "Scene.h"

void                     Scene::AppendGUID(std::string guid) { m_chunk.push_back(guid); }
std::vector<std::string> Scene::GetChunk() const { return m_chunk; }


Scene& Scene::operator=(const Scene& other)
{
    if (this != &other)
        this->m_chunk = other.m_chunk;
    return *this;
}
Scene Scene::operator+(const Scene& other)
{
    Scene newScene;
    newScene.m_chunk = this->m_chunk;
    for (auto& item : other.m_chunk)
        newScene.m_chunk.push_back(item);
    return newScene;
}
Scene& Scene::operator+=(const Scene& other)
{
    for (auto& item : other.m_chunk)
        m_chunk.push_back(item);
    return *this;
}
