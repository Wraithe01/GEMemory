#include "Scene.h"

void                     Scene::AppendGUID(std::string guid) { m_chunk.push_back(guid); }
std::vector<std::string> Scene::GetChunk() const { return m_chunk; }

void Scene::AppendChunk(std::string chunkName)
{
    // Read the JSON file
    std::ifstream file("../PackageFolder/scenes.json");
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON file\n";
        return;
    }
    // Parse JSON
    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return;
    }
    // Get list of guids
    const auto& guid_list = jsonData[chunkName];

    // Append the guids
    for (const auto& guid : guid_list) {
        AppendGUID(guid);
    }
}

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
