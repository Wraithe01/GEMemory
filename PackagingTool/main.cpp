#include "ZipPackaging.h"
#include "TarPackaging.h"
#include "ScenePackaging.h"
#include "microtar.h"
#include <iostream>
#include <filesystem>
#include <string>

int main() {
    std::cout << "============================================================" << std::endl;
    std::cout << "============== PACKAGE AND SCENE CREATION TOOL =============" << std::endl;
    std::cout << "============================================================" << std::endl << std::endl;

    std::string choice;

    std::cout << "Do you want to package a directory (1) or create a scene (2)?" << std::endl;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << std::endl << "============================================================" << std::endl;
        std::cout << "===================== PACKAGE CREATION =====================" << std::endl;
        std::cout << "============================================================" << std::endl << std::endl;

        std::cout << "Enter the path to the folder you want to compress:" << std::endl;
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
        //TarPackagingTool tarPackager;

        //tarPackager.createHeaderFile(assets, headerPath, outputPathTar);

        //tarPackager.createPackage(assets, outputPathTar);
    }
    else if (choice == "2") {

        std::cout << std::endl <<"============================================================" << std::endl;
        std::cout << "====================== SCENE CREATION ======================" << std::endl;
        std::cout << "============================================================" << std::endl << std::endl;

        std::cout << "Enter a path to a directory you want to add to a scene, then press ENTER." << std::endl;
        std::cout << "You can add several directories, write 'x' and ENTER when you are done." << std::endl;
        
        std::string folderPath;
        std::vector<std::string> directories;

        std::string inputJson = "../PackageFolder/header.json";
        std::string outputJson = "../PackageFolder/scenes.json";

        while (folderPath != "x")
        {
            std::getline(std::cin, folderPath);
            if (folderPath == "x") {
                break;
            }
            if (!std::filesystem::is_directory(folderPath)) {
                std::cerr << "Error: Not a valid directory path." << std::endl;
                return 1;
            }

            directories.push_back(folderPath);
        }

        // Execute scene creation
        ScenePackaging scenePackager;
        scenePackager.createPackage(directories, inputJson, outputJson);

    }
    else {
        std::cout << "Error: Enter a valid option (1) or (2).";
        return 1; 
    }

}
