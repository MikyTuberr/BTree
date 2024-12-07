#pragma once
#include "./Config.h"
#include <queue>
#include <sstream>
#include "./Managers/PageManagerConfig.h"
#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"

enum SiblingType { LEFT_SIBLING, RIGHT_SIBLING };
struct SiblingInfo
{
	SiblingInfo(TreeRecord separatorRecord, SiblingType siblingType, std::size_t siblingNumber)
		: separatorRecord(separatorRecord), siblingType(siblingType), siblingNumber(siblingNumber) {}

	TreeRecord separatorRecord;
	SiblingType siblingType;
	std::size_t siblingNumber;
};

class BTree
{
public:
	BTree(PageManagerConfig diskConfig, PageManagerConfig treeConfig, std::size_t paramsNumber)
		: diskPageManager(diskConfig), treePageManager(treeConfig, paramsNumber) {}

	bool InsertRecord(DiskRecord diskRecord);
	TreeRecord FindRecord(std::size_t treeRecordId);

	void Print();
private:

	std::size_t GetLeaf(std::size_t treeRecordId);
	bool TryCompensation(TreePage* currentPage, TreeRecord recordToInsert);
	std::vector<SiblingInfo> DetermineSibling(TreePage* currentPage, TreePage* parentPage);
	TreeRecord SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert);

	DiskPageManager diskPageManager;
	TreePageManager treePageManager;
};