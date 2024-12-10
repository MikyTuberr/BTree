#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"
#include "Config.h"
#include "BTree.h"
#include "InputHandler.h"
#include <fstream>

int main() {
	try {
		// todo: mo¿e wpisywanie nazwy pliku z klawy?

		std::string diskFilename = "./Src/Data/diskTest.bin";
		std::string treeFilename = "./Src/Data/treeTest.bin";
		std::ios_base::openmode diskOpenmode = std::ios::in | std::ios::out | std::ios::binary;
		std::ios_base::openmode treeOpenmode = std::ios::in | std::ios::out | std::ios::binary;

		PageManagerConfig diskPageManagerConfig = PageManagerConfig(DISK_PAGE_SIZE,
			DISK_PAGE_SIZE / DISK_RECORD_SIZE, diskFilename, diskOpenmode);

		PageManagerConfig treePageManagerConfig = PageManagerConfig(TREE_PAGE_SIZE,
			TREE_PAGE_SIZE / TREE_RECORD_SIZE, treeFilename, treeOpenmode);

		BTree bTree(diskPageManagerConfig, treePageManagerConfig, TREE_PAGE_PARAMS_NUMBER);

		InputHandler inputHandler(bTree);
		inputHandler.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what();
	}

	return 0;
}