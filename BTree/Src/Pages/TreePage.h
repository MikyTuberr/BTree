#pragma once
#include "../Records/TreeRecord.h"
#include "../Config.h"

class TreePage
{
public:
	TreePage()
		: pageSize(0), recordsNumber(0), pageNumber(NULLPTR),
		records(std::vector<TreeRecord>()) {}

	TreePage(std::size_t treePageSize, std::size_t treeRecordsNumber, std::size_t treePageNumber)
		: pageSize(treePageSize), recordsNumber(treeRecordsNumber), pageNumber(treePageNumber),
		records(std::vector<TreeRecord>()) {}

	TreePage(std::size_t treePageSize, std::size_t treeRecordsNumber, std::size_t treePageNumber,
		std::size_t leftChildPageNumber, std::vector<TreeRecord>& treeRecords)
		: pageSize(treePageSize), recordsNumber(treeRecordsNumber), pageNumber(treePageNumber),
		headLeftChildPageNumber(leftChildPageNumber), records(treeRecords) {}

	bool InsertRecord(const TreeRecord treeRecord);
	TreeRecord FindRecordById(std::size_t id);
	void RemoveRecordById(std::size_t id);
	std::pair<std::size_t, TreeRecord> FindLeftSiblingNumberById(std::size_t id);
	std::pair<std::size_t, TreeRecord> FindRightSiblingNumberById(std::size_t id);
	TreeRecord PopTail();
	TreeRecord PopHead();
	bool ReplaceRecord(TreeRecord recordToReplace, TreeRecord newRecord);

	bool isOverflow() const;

	const std::size_t GetPageSize() const;
	const std::size_t GetRecordsNumber() const;
	const std::size_t GetPageNumber() const;
	const std::size_t GetHeadLeftChildPageNumber() const;
	const std::size_t GetRightChildPageNumberById(const std::size_t index);
	const std::vector<TreeRecord> GetRecords() const;
	const std::vector<TreeRecord> GetRecordsByIndex(const std::size_t index, const bool direction);
	const std::size_t GetRecordsSize() const;
	const TreeRecord GetRecordByIndex(const std::size_t i) const;
	const std::vector<TreeRecord> GetFixedRecords() const;
	const TreeRecord GetHead() const;

	const void SetRecords(const std::vector<TreeRecord> treeRecords);
	const void SetHeadLeftChildPageNumber(const std::size_t treePageNumber);

	void Print() const;
private:
	std::size_t pageSize;
	std::size_t recordsNumber;
	std::size_t pageNumber;
	std::size_t headLeftChildPageNumber = NULLPTR;
	std::vector<TreeRecord> records;
};