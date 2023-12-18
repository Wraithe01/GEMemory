#include "ZipPackaging.h"
#include "TarPackaging.h"
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
    std::string packageDirectory = "../PackageFolder/";
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

    // Create the zip package
    ZipPackagingTool zipPackager;
    zipPackager.createPackage(assets, outputPath);
    zipPackager.createHeaderFile(assets, headerPath, outputPath);

    // Get paths
    std::string outputPathTar = packageDirectory + folderName.string() + "_package.tar";

    // Create the tar package
    TarPackagingTool tarPackager;

    tarPackager.createHeaderFile(assets, headerPath, outputPathTar);

    tarPackager.createPackage(assets, outputPathTar);


    return 0;
}