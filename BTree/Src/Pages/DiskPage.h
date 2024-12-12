#pragma once
#include "../Records/DiskRecord.h"
#include "../Config.h"

class DiskPage
{
public:
	DiskPage()
		: pageSize(0), recordsNumber(0), pageNumber(NULLPTR), 
		records(std::vector<DiskRecord>()) {}

	DiskPage(std::size_t pageSize, std::size_t recordsNumber, std::size_t diskPageNumber)
		: pageSize(pageSize), recordsNumber(recordsNumber), pageNumber(diskPageNumber), 
		records(std::vector<DiskRecord>()) {}

	DiskPage(std::size_t pageSize, std::size_t recordsNumber, std::size_t diskPageNumber,
		std::vector<DiskRecord>& diskRecords)
		: pageSize(pageSize), recordsNumber(recordsNumber), pageNumber(diskPageNumber),
		records(diskRecords) {}

	const std::size_t GetPageSize() const;
	const std::size_t GetRecordsNumber() const;
	const std::size_t GetPageNumber() const;
	const std::vector<DiskRecord> GetRecords() const;
	const std::vector<DiskRecord> GetFixedRecords() const;

	const void SetRecords(const std::vector<DiskRecord> diskRecords);

	bool InsertRecord(const DiskRecord diskRecord);
	DiskRecord* FindRecordById(std::size_t id);
	void RemoveRecordById(std::size_t id);

	bool isOverflow() const;

	void Print() const;
private:
	std::size_t pageSize;
	std::size_t recordsNumber;
	std::size_t pageNumber;
	std::vector<DiskRecord> records;
};

