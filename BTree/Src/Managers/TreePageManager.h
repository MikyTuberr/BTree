#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/TreePage.h"
#include <string>
#include "PageManagerConfig.h"

class TreePageManager
{
public:
	TreePageManager(PageManagerConfig config, std::size_t paramsNumber)
		: root(config.pageSize, config.pageRecordsNumber, UINT_MAX), 
		randomAccessFile(config.filename, config.openmode), paramsNumber(paramsNumber) {}


	TreePage ReadPage(std::size_t pageNumber);
	bool WritePage(const TreePage& treePage);

	TreePage GetRoot() const;

	bool InsertRecord(TreeRecord treeRecord, TreePage treePage);
	TreeRecord* FindRecordInPageById(TreePage& page, const std::size_t& id);
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	TreePage root;
	std::size_t paramsNumber;
	std::size_t pagesCounter = 0;
};


