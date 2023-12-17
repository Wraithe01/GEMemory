#include "TarPackaging.h"
#include "zlib.h"

void TarPackagingTool::createPackage(std::vector<std::string>& assets, const std::string& outputPath) {
    // Open/create tar file
    tar::tar_writer tarWriter(outputPath);

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
        if (inputFile.read(buffer.data(), size) || (inputFile.gcount() == size)) {
            // Extract filename
            std::filesystem::path assetPath(asset);
            std::string filename = assetPath.filename().string();

            // Add the asset to the tar file
            tarWriter.write(filename, buffer.data(), static_cast<size_t>(size));
        }
        else {
            std::cerr << "Failed to read asset file: " << asset << std::endl;
        }

        inputFile.close();
    }

    std::cout << "Done creating tar package at: " << outputPath << std::endl;
}
