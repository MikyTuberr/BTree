#pragma once
#include "./Config.h"
#include <queue>
#include "./Managers/PageManagerConfig.h"
#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"

enum SiblingType { LEFT_SIBLING, RIGHT_SIBLING, NO_SIBLING };

class BTree
{
public:
	BTree(PageManagerConfig diskConfig, PageManagerConfig treeConfig, std::size_t rootNumber, 
		std::size_t paramsNumber)
		: diskPageManager(diskConfig), treePageManager(treeConfig, rootNumber, paramsNumber) {}

	bool InsertRecord(DiskRecord diskRecord);
	TreeRecord FindRecord(std::size_t treeRecordId);

	void Print();
private:

	std::size_t GetLeaf(std::size_t treeRecordId);
	bool TryCompensation(TreePage* currentPage, TreeRecord recordToInsert);
	std::pair<SiblingType, std::pair<std::size_t, TreeRecord>> DetermineSibling(TreePage* currentPage, TreePage* parentPage);
	TreeRecord SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert);

	DiskPageManager diskPageManager;
	TreePageManager treePageManager;

};