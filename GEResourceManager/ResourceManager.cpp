#include "ResourceManager.h"

void ResourceManager::LoadHeader()
{
    // Read the JSON file
    std::ifstream file("../Packages/header.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file\n";
        return;
    }
    // Parse JSON
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return;
    }

    for (const auto& entry : jsonData.items()) {
        const std::string& guid = entry.key();
        const std::string& filename = entry.value()["filename"];
        const std::string& package = entry.value()["package"];
        const uLong offset = entry.value()["filepos"];
        const uLong fileNumber = entry.value()["filenumber"];
        const unz_file_pos_s filePos{ offset, fileNumber };

        HeaderEntry entryData{ filename, package, filePos };
        headerMap[guid] = entryData;
    }
}

std::string ResourceManager::GetPackage(const std::string& guid)
{
    const auto& it = headerMap.find(guid);

    if (it != headerMap.end())
    {
        return it->second.package;
    }
    return "";
}