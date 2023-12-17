#include "PackagingTool.h"
#include "json.hpp"
#include "tar.hpp"

void PackagingTool::generateGUID(GUID& guid) {
    CoCreateGuid(&guid); // Generates a guid
}

void PackagingTool::createHeaderFile(std::vector<std::string>& assets, const std::string& jsonFilePath, const std::string& packageName) {
    nlohmann::json json; // Create a json object

    // Check if it exists
    std::ifstream existingJsonFile(jsonFilePath);
    if (existingJsonFile.is_open()) {
        existingJsonFile >> json;
        existingJsonFile.close();
    }

    // Add data to json
    for (auto& asset : assets) {
        GUID guid;
        generateGUID(guid);

        // Extract the file name
        std::filesystem::path assetPath(asset);
        std::string filename = assetPath.stem().string();
        std::string filetype = assetPath.extension().string();

        // Check if name and package combination exists
        bool assetExists = false;
        for (const auto& entry : json.items()) {
            const auto& existingAsset = entry.value();
            if (existingAsset["filename"] == filename && existingAsset["package"] == packageName) {
                assetExists = true;
            }
        }

        if (!assetExists) {
            // Convert guid to a string
            std::stringstream guidStream;
            guidStream << std::hex
                << guid.Data1 << '-'
                << guid.Data2 << '-'
                << guid.Data3 << '-'
                << static_cast<short>(guid.Data4[0])
                << static_cast<short>(guid.Data4[1]) << '-'
                << static_cast<short>(guid.Data4[2])
                << static_cast<short>(guid.Data4[3])
                << static_cast<short>(guid.Data4[4])
                << static_cast<short>(guid.Data4[5])
                << static_cast<short>(guid.Data4[6])
                << static_cast<short>(guid.Data4[7]);

            // Add items to json
            nlohmann::json item;
            item["filetype"] = filetype.substr(1);
            item["package"] = packageName;

            std::filesystem::path packagePath(packageName);
            std::string packageType = packagePath.extension().string();
            
            if (packageType == ".zip") {
                item["filename"] = filename;
                // Get filePos struct and add to json
                unzFile zipFile = unzOpen(packageName.c_str());
                if (zipFile != nullptr) {
                    if (unzLocateFile(zipFile, assetPath.filename().string().c_str(), 1) != UNZ_OK) {
                        unzClose(zipFile);
                    }

                    unz_file_pos filePos;
                    if (unzGetFilePos(zipFile, &filePos) == UNZ_OK) {
                        item["offset"] = filePos.pos_in_zip_directory;
                        item["filenumber"] = filePos.num_of_file;
                    }

                    unzClose(zipFile);
                }
            }
            else if (packageType == ".tar") {
                std::string guidFileName = guidStream.str() + filetype;
                item["filename"] = guidFileName;
                item["offset"] = 0;
                item["filenumber"] = 0;

                // File renaming quickfix (should be able to do it better but no time rn)
                std::string oldFilePath = assetPath.string();
                std::string newFileName = guidFileName;
                std::filesystem::path directoryPath = std::filesystem::path(oldFilePath).parent_path();
                std::filesystem::path newFilePath = directoryPath / newFileName;
                std::filesystem::rename(oldFilePath, newFilePath);
                
                asset = newFilePath.string();

            }
            json[guidStream.str()] = item;
        }
    }

    // Write to json
    std::ofstream jsonFile(jsonFilePath, std::ios::trunc);
    if (jsonFile.is_open()) {
        jsonFile << std::setw(4) << json;
        jsonFile.close();
    }
}
