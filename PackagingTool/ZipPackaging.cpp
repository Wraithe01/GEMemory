#include "ZipPackaging.h"

void ZipPackagingTool::createPackage(const std::vector<std::string>& assets, const std::string& outputPath) {
    // Open/create zip file
    zipFile zip = zipOpen(outputPath.c_str(), APPEND_STATUS_CREATE);

    if (zip == nullptr) {
        std::cerr << "Failed to create zip file: " << outputPath << std::endl;
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
        //heeeej
        // Get the size of the asset file
        inputFile.seekg(0, std::ios::end);
        std::streamsize size = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        // Read the content of the asset file
        std::vector<char> buffer(static_cast<size_t>(size));
        if (inputFile.read(buffer.data(), size)) {
            // Extract filename
            size_t lastPathSeparator = asset.find_last_of("/\\");
            std::string filename = asset.substr(lastPathSeparator + 1);

            // Add the asset to the zip
            zip_fileinfo fileInfo{};
            zipOpenNewFileInZip(zip, filename.c_str(), &fileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            zipWriteInFileInZip(zip, buffer.data(), static_cast<unsigned int>(size));
            zipCloseFileInZip(zip);
        }
        else {
            std::cerr << "Failed to read asset file: " << asset << std::endl;
        }

        inputFile.close();
    }
    std::cout << "Done creating zip package at: " << outputPath << std::endl;

    // Close the zip
    zipClose(zip, nullptr);
}