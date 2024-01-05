#include "TarPackaging.h"

void TarPackagingTool::createPackage(std::vector<std::string>& assets, const std::string& outputPath) {
    // Open/create tar file
    mtar_t tar;

    if (mtar_open(&tar, outputPath.c_str(), "w") != MTAR_ESUCCESS) {
        std::cerr << "Failed to create tar file: " << outputPath << std::endl;
        return;
    }

    // Add the assets
    for (const std::string& asset : assets) {
        // Open each asset file
        std::ifstream inputFile(asset, std::ios::binary);

        if (!inputFile.is_open()) {
            std::cerr << "Failed to open asset file: " << asset << std::endl;
            continue;
        }

        // Get the size of the asset file
        inputFile.seekg(0, std::ios::end);
        std::streamsize size = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        // Read the content of the asset file
        std::vector<char> buffer(static_cast<size_t>(size));
        if (inputFile.read(buffer.data(), size)) {
            // Extract filename
            std::filesystem::path assetPath(asset);
            std::string filename = assetPath.filename().string();

            // Add the asset to the tar 
            mtar_write_file_header(&tar, filename.c_str(), static_cast<size_t>(size));
            mtar_write_data(&tar, buffer.data(), static_cast<size_t>(size));
        }
        else {
            std::cerr << "Failed to read asset file: " << asset << std::endl;
        }

        inputFile.close();
    }
    std::cout << std::endl << "Success: Done creating tar package at: " << outputPath << std::endl;

    // Close the tar
    mtar_finalize(&tar);
    mtar_close(&tar);
}
