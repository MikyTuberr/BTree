#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/TreePage.h"
#include <string>
#include "PageManagerConfig.h"

class TreePageManager
{
public:
	TreePageManager(PageManagerConfig config, std::size_t paramsNumber)
		: root(config.pageSize, config.pageRecordsNumber, 0), 
		randomAccessFile(config.filename, config.openmode), paramsNumber(paramsNumber) {}


	TreePage ReadPage(std::size_t pageNumber);
	template <typename Param>
	bool WritePage(const TreePage& treePage, const std::vector<Param>& params);

	TreePage GetRoot() const;

	bool InsertRecord(const TreeRecord treeRecord);
	TreeRecord* FindRecordInPageById(const TreePage& page, const std::size_t& id) const;
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	TreePage root;
	std::size_t paramsNumber;
	std::size_t pagesCounter = 0;
};

template<typename Param>
inline bool TreePageManager::WritePage(const TreePage& treePage, const std::vector<Param>& params)
{
	if (treePage.GetPageNumber() == this->pagesCounter) {
		this->pagesCounter++;
	}

	std::streampos cursor = this->CalculateCursor(treePage.GetPageNumber());
	std::vector<TreeRecord> fixedRecords = treePage.GetFixedRecords();
	return this->randomAccessFile.WriteRecords(fixedRecords, cursor, std::ios_base::beg, params);
}
