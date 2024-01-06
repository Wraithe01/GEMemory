#pragma once
#include "Includes.h"
#include <fstream>
#include <json.hpp>

class Scene sealed
{
public:  // Methods
    Scene()  = default;
    ~Scene() = default;

    // Simulating appending a game object with multiple GUIDs at once
    void AppendGUID(std::string guid);
    std::vector<std::string> GetChunk() const;

    void AppendScene(std::string sceneName);

    // Use these to test functionality
    Scene& operator=(const Scene& other);
    Scene  operator+(const Scene& other);
    Scene& operator+=(const Scene& other);

private:
private:  // Variables
    std::vector<std::string> m_chunk;
};
