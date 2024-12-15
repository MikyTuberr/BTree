#include "FileHandler.h"

void FileHandler::prepareFiles(std::string& diskFilename, std::string& treeFilename,
    std::ios_base::openmode& diskOpenmode, std::ios_base::openmode& treeOpenmode) 
{
    listFilesInDirectory();

    askAndPrepareFilePath(diskFilename, "disk file", diskOpenmode);
    askAndPrepareFilePath(treeFilename, "tree file", treeOpenmode);
}

void FileHandler::validateAndPrefixFilename(std::string& filename, const std::string& fileType)
{
    if (fileType == "disk file" && filename.substr(0, 2) != "d_") {
        std::cout << "Prefix 'd_' was missing for the disk file. Adding the prefix to the filename: " << filename << std::endl;
        filename = "d_" + filename;
    }
    else if (fileType == "tree file" && filename.substr(0, 2) != "t_") {
        std::cout << "Prefix 't_' was missing for the tree file. Adding the prefix to the filename: " << filename << std::endl;
        filename = "t_" + filename;
    }
}

void FileHandler::listFilesInDirectory()
{
    std::string directoryPath = "./Src/Data/";

    try {
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
            std::cout << "Existing files in " << directoryPath << ":\n";

            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (std::filesystem::is_regular_file(entry.status())) {
                    std::string filename = entry.path().filename().string();
                    if (filename.substr(0, 2) == "d_") {
                        std::cout << " - " << filename << " (disk file)" << std::endl;
                    }
                    else if (filename.substr(0, 2) == "t_") {
                        std::cout << " - " << filename << " (tree file)" << std::endl;
                    }
                    else {
                        std::cout << " - " << filename << " (unknown file type)" << std::endl;
                    }
                }
            }
        }
        else {
            std::cout << "Directory " << directoryPath << " does not exist.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error while accessing the directory: " << e.what() << std::endl;
    }
}

bool FileHandler::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void FileHandler::askAndPrepareFilePath(std::string& filename, const std::string& fileType, std::ios_base::openmode& openmode) {
    if (fileType == "disk file") {
        std::cout << "Enter the path for the " << fileType << " (e.g., d_test.bin). Please ensure the prefix 'd_' is included.\n> ";
    }
    else if (fileType == "tree file") {
        std::cout << "Enter the path for the " << fileType << " (e.g., t_test.bin). Please ensure the prefix 't_' is included.\n> ";
    }
    std::getline(std::cin, filename);
    validateAndPrefixFilename(filename, fileType);
    filename = "./Src/Data/" + filename;

    openmode = std::ios::in | std::ios::out | std::ios::binary;

    if (fileExists(filename)) {
        std::cout << "The " << fileType << " " << filename << " exists, opening in read-write mode..." << std::endl;
    }
    else {
        std::cout << "The " << fileType << " " << filename << " does not exist, creating a new one..." << std::endl;
        std::ofstream newFile(filename, std::ios::out | std::ios::binary);
        newFile.close();
    }
}