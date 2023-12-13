#include "PackagingTool.h"

void PackagingTool::generateGUID(GUID& guid) {
    CoCreateGuid(&guid); // Generates a guid
}

void PackagingTool::createHeaderFile(const std::vector<std::string>& assets, const std::string& headerFilePath) {
    std::ofstream mappingFile(headerFilePath);
    if (mappingFile.is_open()) {
        for (const auto& asset : assets) {
            GUID guid;
            generateGUID(guid);

            // Extract the file name
            size_t lastPathSeparator = asset.find_last_of("/\\");
            std::string filename = (lastPathSeparator != std::string::npos) ? asset.substr(lastPathSeparator + 1) : asset;

            // Writes the guid to the txt-file
            mappingFile << std::hex
                << guid.Data1
                << '-' << guid.Data2
                << '-' << guid.Data3
                << '-' << static_cast<short>(guid.Data4[0])
                << static_cast<short>(guid.Data4[1])
                << '-' << static_cast<short>(guid.Data4[2])
                << static_cast<short>(guid.Data4[3])
                << static_cast<short>(guid.Data4[4])
                << static_cast<short>(guid.Data4[5])
                << static_cast<short>(guid.Data4[6])
                << static_cast<short>(guid.Data4[7])
                << ": " << filename << std::endl;
        }

        mappingFile.close();
    }
}