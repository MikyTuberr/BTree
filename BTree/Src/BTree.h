#pragma once
#include "./Config.h"
#include <queue>
#include "./Managers/PageManagerConfig.h"
#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"

class BTree
{
public:
	BTree(PageManagerConfig diskConfig, PageManagerConfig treeConfig, std::size_t rootNumber, 
		std::size_t paramsNumber)
		: diskPageManager(diskConfig), treePageManager(treeConfig, rootNumber, paramsNumber),
		minKeysPerPage(treeConfig.pageRecordsNumber), maxKeysPerPage(2 * treeConfig.pageRecordsNumber) {}

	bool InsertRecord(DiskRecord diskRecord);
	TreeRecord FindRecord(std::size_t treeRecordId);
	std::size_t GetLeaf(std::size_t treeRecordId);
	bool TryCompensation(TreePage leafPage, TreeRecord recordToInsert);
	TreeRecord SplitPage(TreePage pageToSplit, TreeRecord recordToInsert);

	std::size_t GetRootNumber();

	void Print();
private:
	DiskPageManager diskPageManager;
	TreePageManager treePageManager;
	std::size_t minKeysPerPage;
	std::size_t maxKeysPerPage;
};