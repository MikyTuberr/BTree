#pragma once
#include "./Managers/PageManagerConfig.h"
#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"

class BTree
{
public:
	BTree(PageManagerConfig diskConfig, PageManagerConfig treeConfig, std::size_t paramsNumber)
		: diskPageManager(diskConfig), treePageManager(treeConfig, paramsNumber),
		minKeysPerPage(treeConfig.pageRecordsNumber), maxKeysPerPage(2 * treeConfig.pageRecordsNumber) {
	}

	bool InsertRecord(DiskRecord diskRecord);
	std::pair<TreeRecord*, std::size_t> FindRecord(std::size_t treeRecordId);

	void Print();
private:
	DiskPageManager diskPageManager;
	TreePageManager treePageManager;
	std::size_t minKeysPerPage;
	std::size_t maxKeysPerPage;
};