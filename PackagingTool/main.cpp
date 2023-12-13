#include "ZipPackaging.h"
#include <iostream>
#include <filesystem>
#include <string>

int main() {
    std::cout << "Enter the path to the folder you want to compress: ";
    std::string folderPath;
    std::getline(std::cin, folderPath);

    if (!std::filesystem::is_directory(folderPath)) {
        std::cerr << "Error: Not a valid directory path." << std::endl;
        return 1;
    }

    // Get paths
    std::filesystem::path folderName = std::filesystem::path(folderPath).filename();
    std::string outputPath = "../Packages/" + folderName.string() + "_package.zip";
    std::string headerPath = "../Packages/" + folderName.string() + "_header.txt";

    // Add files to vector from the folder
    std::vector<std::string> assets;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        assets.push_back(entry.path().string());
    }

    // Create zip or tar? option... :D

    // Create the ZIP package
    ZipPackagingTool zipPackager;
    zipPackager.createPackage(assets, outputPath);
    zipPackager.createMappingFile(assets, headerPath);

    // TODO
    // 
    // Create the TAR package
    // TarPackagingTool tarPackager;
    // tarPackager.createPackage(assets, outputPath);
    // tarPackager.createMappingFile(assets, headerPath);

    return 0;
}