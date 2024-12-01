#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/TreePage.h"
#include <string>
#include "PageManagerConfig.h"

class TreePageManager
{
public:
	TreePageManager(PageManagerConfig config, std::size_t rootNumber, std::size_t paramsNumber)
		: pageSize(config.pageSize), rootNumber(rootNumber), pageRecordsNumber(config.pageRecordsNumber), 
		randomAccessFile(config.filename, config.openmode), paramsNumber(paramsNumber) 
	{
		if (rootNumber == NULLPTR) {
			this->rootNumber = 0;
			this->WritePage(TreePage(pageSize, pageRecordsNumber, this->rootNumber));
		}
	}


	//TODO: zapisywanie wskaznika na root i WCZYTYWANIE! ~TreePageManager();

	TreePage ReadPage(std::size_t pageNumber);
	bool WritePage(const TreePage& treePage);
	TreePage CreateNewPage();

	TreePage GetRoot();
	const std::size_t GetRootNumber() const;

	const void SetRootNumber(std::size_t rootNumber);

	bool InsertRecord(TreeRecord treeRecord, TreePage treePage);
	TreeRecord FindRecordInPageById(TreePage& page, const std::size_t& id);
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	std::size_t rootNumber;
	std::size_t pageSize;
	std::size_t pageRecordsNumber;
	std::size_t paramsNumber;
	std::size_t pagesCounter = 0;
};


