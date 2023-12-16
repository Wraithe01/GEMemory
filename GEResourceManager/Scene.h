#pragma once
#include "Includes.h"

class Scene sealed
{
public:  // Methods
    Scene()  = default;
    ~Scene() = default;

    // Simulating appending a game object with multiple GUIDs at once
    void AppendGUID(std::string guid);

    std::vector<std::string> GetChunk() const;

    // Use these to test functionality
    Scene& operator=(const Scene& other);
    Scene  operator+(const Scene& other);
    Scene& operator+=(const Scene& other);

private:
private:  // Variables
    std::vector<std::string> m_chunk;
};
