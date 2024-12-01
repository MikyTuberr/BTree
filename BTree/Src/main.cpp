#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"
#include "Config.h"
#include "BTree.h"
#include <fstream>

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

		/*DiskPageManager diskPageManager(DISK_PAGE_SIZE, DISK_PAGE_SIZE / DISK_RECORD_SIZE, diskFilename, diskOpenmode);

		for (int i = 0; i < DISK_PAGE_SIZE / DISK_RECORD_SIZE + 1; i++) {
			diskPageManager.InsertRecordToBuffer(DiskRecord(i, i, i));
		}

		DiskPage diskPage = diskPageManager.ReadPage(0);
		diskPage.Print();
		diskPage = diskPageManager.ReadPage(1);
		diskPage.Print();

		diskPage.InsertRecord(DiskRecord(2130, 432243, 3424));
		diskPageManager.WritePage(diskPage);

		diskPage = diskPageManager.ReadPage(0);
		diskPage.Print();
		diskPage = diskPageManager.ReadPage(1);
		diskPage.Print();*/

		/*TreePageManager treePageManager(TREE_PAGE_SIZE, TREE_PAGE_SIZE / TREE_RECORD_SIZE, treeFilename, treeOpenmode, TREE_PAGE_PARAMS_NUMBER);

		for (int i = 0; i < TREE_PAGE_SIZE / TREE_RECORD_SIZE + 1; i++) {
			treePageManager.InsertRecord(TreeRecord(i, i, i));


		TreePage treePage = treePageManager.ReadPage(0);
		treePage.Print();
		treePage = treePageManager.ReadPage(1);
		treePage.Print();

		treePage.InsertRecord(DiskRecord(2130, 432243, 3424));
		treePageManager.WritePage(treePage);

		treePage = treePageManager.ReadPage(0);
		treePage.Print();
		treePage = treePageManager.ReadPage(1);
		treePage.Print();*/

		PageManagerConfig diskPageManagerConfig = PageManagerConfig(DISK_PAGE_SIZE, 
			DISK_PAGE_SIZE / DISK_RECORD_SIZE, diskFilename, diskOpenmode);

		PageManagerConfig treePageManagerConfig = PageManagerConfig(TREE_PAGE_SIZE,
			TREE_PAGE_SIZE / TREE_RECORD_SIZE, treeFilename, treeOpenmode);

		BTree bTree(diskPageManagerConfig, treePageManagerConfig, TREE_PAGE_PARAMS_NUMBER);

		for (int i = 0; i < DISK_PAGE_SIZE / DISK_RECORD_SIZE + 1; i++) {
			bTree.InsertRecord(DiskRecord(i, i, i, i));
		}
		bTree.FindRecord(3).first->Print();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what();
	}

	return 0;
}