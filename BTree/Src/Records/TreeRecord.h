#pragma once
#include <iostream>
#include "../Type.h"

class TreeRecord
{
public:
	TreeRecord()
		: diskRecordId(NULLPTR), diskRecordPageNumber(NULLPTR), 
		treeRightChildNumber(NULLPTR) {
	}

	TreeRecord(std::size_t id, std::size_t recordPageNumber)
		: diskRecordId(id), diskRecordPageNumber(recordPageNumber), 
		treeRightChildNumber(NULLPTR) {
	}

	TreeRecord(std::size_t id, std::size_t recordPageNumber, std::size_t childPageNumber)
		: diskRecordId(id), diskRecordPageNumber(recordPageNumber), treeRightChildNumber(childPageNumber) {
	}

	std::size_t GetId() const;
	std::size_t GetTreeRightChildNumber() const;
	std::size_t GetDiskPageNumber() const;

	void SetTreeRightChildNumber(const std::size_t childTreePageNumber);

	void Print() const;
private:
	std::size_t diskRecordId;
	std::size_t diskRecordPageNumber;
	std::size_t treeRightChildNumber;
};