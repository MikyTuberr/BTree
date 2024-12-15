#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>

class FileHandler {
public:
    static void prepareFiles(std::string& diskFilename, std::string& treeFilename,
        std::ios_base::openmode& diskOpenmode, std::ios_base::openmode& treeOpenmode);

private:
    static void validateAndPrefixFilename(std::string& filename, const std::string& fileType);
    static void listFilesInDirectory();
    static bool fileExists(const std::string& filename);
    static void askAndPrepareFilePath(std::string& filename, const std::string& fileType, std::ios_base::openmode& openmode);
};
