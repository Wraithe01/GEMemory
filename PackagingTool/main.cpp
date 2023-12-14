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
    std::string packageDirectory = "../Packages/";
    std::string outputPath = packageDirectory + folderName.string() + "_package.zip";
    std::string headerPath = packageDirectory + "header.json";

    // Create packages folder if it does not exist
    if (!std::filesystem::exists(packageDirectory)) {
        std::filesystem::create_directory(packageDirectory);
    }

    // Add files to vector from the folder
    std::vector<std::string> assets;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        assets.push_back(entry.path().string());
    }

    // Create zip or tar? option... :D

    // Create the ZIP package
    ZipPackagingTool zipPackager;
    zipPackager.createPackage(assets, outputPath);
    zipPackager.createHeaderFile(assets, headerPath, outputPath);

    // TODO
    // 
    // Create the TAR package
    // TarPackagingTool tarPackager;
    // tarPackager.createPackage(assets, outputPath);
    // tarPackager.createHeaderFile(assets, headerPath);

    return 0;
}