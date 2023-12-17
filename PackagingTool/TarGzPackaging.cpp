#include "TarGzPackaging.h"
#include "zlib.h"
#include "zstr.hpp"
void TarGzPackagingTool::createPackage(const std::vector<std::string>& assets, const std::string& outputPath) {
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
}

void TarGzPackagingTool::gzipTarFile(const std::string& tarFile, const std::string& outputPath) {
    // Open the tar file
    std::ifstream ifs(tarFile, std::ios::binary);

    if (!ifs.is_open()) {
        std::cerr << "Error opening input file: " << tarFile << std::endl;
        return;
    }

    // Open/create tar.gz file
    zstr::ofstream gzout(outputPath, std::ios::binary);
    if (!gzout.is_open()) {
        std::cerr << "Error opening output file: " << outputPath << std::endl;
        ifs.close();
        return;
    }

    // Compress and write
    gzout << ifs.rdbuf();

    // Close files
    ifs.close();
    gzout.close();

    // Remove the original .tar-file
    std::filesystem::remove(tarFile);

    std::cout << "Done creating tar.gz package at: " << outputPath << std::endl;
}