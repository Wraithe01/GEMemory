#include "ScenePackaging.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

void ScenePackaging::createPackage(std::vector<std::string>& directories, const std::string& inputPath, const std::string& outputPath)
{
    std::ifstream jsonFile(inputPath);
    if (!jsonFile.is_open()) {
        std::cerr << "Error: Failed to open the JSON file." << std::endl;
        return;
    }

    nlohmann::json inputJson;
    jsonFile >> inputJson;

    std::vector<std::string> sceneGuids;

    for (const auto& dir : directories) {
        std::vector<std::string> selectedGuids;
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            std::string fileName = entry.path().filename().string();
           
            for (auto json = inputJson.begin(); json != inputJson.end(); ++json) {
                // Access the GUID and its corresponding data
                std::string guid = json.key();
                std::string filename = json.value()["filename"];
                std::string filetype = json.value()["filetype"];

                if (filename + "." + filetype == fileName) {
                    bool found = false;
                    for (auto i : sceneGuids) {
                        if (i == guid) {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                        sceneGuids.push_back(guid);
                }

            }

        }
    }

    nlohmann::json outputJson;
    std::ifstream existingOutputJsonFile(outputPath);
    if (existingOutputJsonFile.is_open()) {
        existingOutputJsonFile >> outputJson;
        existingOutputJsonFile.close();
    }
    
    std::string newScene = "scene" + std::to_string(outputJson.size() + 1);
    outputJson[newScene] = sceneGuids;

    std::ofstream outputJsonFile(outputPath, std::ios::trunc);
    if (outputJsonFile.is_open()) {
        outputJsonFile << std::setw(4) << outputJson << std::endl;
        outputJsonFile.close();
    }

    std::cout << std::endl << "Success: Done creating scene: " << newScene << std::endl;
}