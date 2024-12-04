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
		: diskPageManager(diskConfig), treePageManager(treeConfig, rootNumber, paramsNumber) {}

	bool InsertRecord(DiskRecord diskRecord);
	TreeRecord FindRecord(std::size_t treeRecordId);
	std::size_t GetLeaf(std::size_t treeRecordId);
	bool TryCompensation(TreePage leafPage, TreeRecord recordToInsert);
	std::pair<SiblingType, std::pair<std::size_t, TreeRecord>> DetermineSibling(TreePage currentPage, TreePage parentPage);
	TreeRecord SplitPage(TreePage pageToSplit, TreeRecord recordToInsert);

	std::size_t GetRootNumber();

	void Print();
private:
	DiskPageManager diskPageManager;
	TreePageManager treePageManager;

};