#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"
#include "Config.h"
#include "BTree.h"
#include "./Handlers/InputHandler.h"
#include "./Handlers/FileHandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>


int main() {
    try {
        std::string diskFilename, treeFilename;
        std::ios_base::openmode diskOpenmode, treeOpenmode;

        FileHandler::prepareFiles(diskFilename, treeFilename, diskOpenmode, treeOpenmode);

        PageManagerConfig diskPageManagerConfig = PageManagerConfig(DISK_PAGE_SIZE,
            DISK_PAGE_SIZE / DISK_RECORD_SIZE, diskFilename, diskOpenmode);

        PageManagerConfig treePageManagerConfig = PageManagerConfig(TREE_PAGE_SIZE,
            TREE_PAGE_SIZE / TREE_RECORD_SIZE, treeFilename, treeOpenmode);

        BTree bTree(diskPageManagerConfig, treePageManagerConfig, TREE_PAGE_PARAMS_NUMBER, d);

        InputHandler inputHandler(bTree);
        inputHandler.runShell();
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error occured: " << e.what() << std::endl;
    }

    return 0;
}
