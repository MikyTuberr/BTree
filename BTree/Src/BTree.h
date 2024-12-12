#pragma once
#include "./Config.h"
#include <queue>
#include <map>
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
	BTree(PageManagerConfig diskConfig, PageManagerConfig treeConfig, std::size_t paramsNumber, std::size_t d)
		: diskPageManager(diskConfig), treePageManager(treeConfig, paramsNumber), d(d){}


	bool InsertRecord(DiskRecord diskRecord);
	std::pair<TreeRecord, std::size_t> FindRecord(std::size_t treeRecordId);
	bool DeleteRecord(std::size_t treeRecordId);

	void PrintDiskFile();

	void Print();
private:

	std::size_t GetLeaf(std::size_t treeRecordId);
	bool TryCompensation(TreePage* currentPage, TreeRecord recordToInsert);
	bool TryCompensation(TreePage* currentPage);
	std::vector<SiblingInfo> DetermineSibling(TreePage* parentPage, std::size_t currentPageNumber);
	TreeRecord SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert);
	std::size_t MergePage(TreePage* pageToMerge);
	std::pair<TreeRecord, std::size_t> FindMinInRightSubtree(std::size_t pageNumber);
	std::size_t d;

	DiskPageManager diskPageManager;
	TreePageManager treePageManager;
};