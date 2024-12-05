#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"
#include "Config.h"
#include "BTree.h"
#include <fstream>

size_t readSizeTFromFile(const std::string& filename) {
	std::ifstream inFile(filename, std::ios::binary);
	if (!inFile) {
		throw std::runtime_error("Cannot open file for reading");
	}
	size_t number;
	inFile.read(reinterpret_cast<char*>(&number), sizeof(size_t));
	inFile.close();
	return number;
}

void saveSizeTToFile(const std::string& filename, size_t number) {
	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile) {
		throw std::runtime_error("Cannot open file for writing");
	}
	outFile.write(reinterpret_cast<const char*>(&number), sizeof(size_t));
	outFile.close();
}

int main() {
	try {
		// todo: menu z opcjami (jakiœ handler inputu)
		// todo: mo¿e wpisywanie nazwy pliku z klawy?
		// todo: wczytywanie pliku testowego
		// todo: prze³¹czanie trybów programu
		// todo: losowe rekordy i z klawy

		std::string diskFilename = "./Src/Data/diskTest.bin";
		std::string treeFilename = "./Src/Data/treeTest.bin";
		std::ios_base::openmode diskOpenmode = std::ios::in | std::ios::out | std::ios::binary;
		std::ios_base::openmode treeOpenmode = std::ios::in | std::ios::out | std::ios::binary;

		PageManagerConfig diskPageManagerConfig = PageManagerConfig(DISK_PAGE_SIZE, 
			DISK_PAGE_SIZE / DISK_RECORD_SIZE, diskFilename, diskOpenmode);

		PageManagerConfig treePageManagerConfig = PageManagerConfig(TREE_PAGE_SIZE,
			TREE_PAGE_SIZE / TREE_RECORD_SIZE, treeFilename, treeOpenmode);


		std::string rootFilename = "./Src/Data/rootTest.bin";

		//std::size_t rootNumber = readSizeTFromFile(rootFilename);

		BTree bTree(diskPageManagerConfig, treePageManagerConfig, NULLPTR,
			TREE_PAGE_PARAMS_NUMBER);

		for (int i = 0; i < 20; i++) {
			DiskRecord record = DiskRecord(i, i, i);
			bTree.InsertRecord(record);
			std::cout << "\n\n============================ " << i << " ============================\n"; 
			std::cout << "Inserting: " << record.GetId() << "\n\n";
			bTree.Print();
		}

		//saveSizeTToFile(rootFilename, bTree.GetRootNumber());
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what();
	}

	return 0;
}