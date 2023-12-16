#include "ZipPackaging.h"
#include "TarGzPackaging.h"
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

    // Create zip or tar? option... :D

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

    // Create the zip package
    ZipPackagingTool zipPackager;
    zipPackager.createPackage(assets, outputPath);
    zipPackager.createHeaderFile(assets, headerPath, outputPath);

    std::string outputPathtest = packageDirectory + folderName.string() + "_package.tar.gz";

    // Create the tar package
    //TarGzPackagingTool tarPackager;
    //tarPackager.createPackage(assets, outputPathtest);
    // FIX createHeader... :D
    //tarPackager.createHeaderFile(assets, headerPath, outputPathtest);

    // libarchive !!!!!!

    return 0;
}