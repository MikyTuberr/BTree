#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/TreePage.h"
#include <string>
#include "PageManagerConfig.h"
#include <unordered_map>

class TreePageManager
{
public:
	TreePageManager(PageManagerConfig config, std::size_t paramsNumber)
		: pageSize(config.pageSize), pageRecordsNumber(config.pageRecordsNumber), 
		randomAccessFile(config.filename, config.openmode), paramsNumber(paramsNumber) 
	{
		if (this->randomAccessFile.isFileEmpty()) {
			this->rootNumber = 0;
			this->pagesCounter = 0;
			//this->randomAccessFile.WriteRecords<std::size_t>({ this->rootNumber, this->pagesCounter }, 0, std::ios::beg);
			TreePage root = TreePage(this->pageSize, this->pageRecordsNumber, this->rootNumber);
			this->WritePage(root);
			pagesWrittenCounter--;
		}
		else {
			auto vec = this->randomAccessFile.ReadRecords<std::size_t>(0, std::ios::beg, TREE_FILE_PARAMS_NUMBER);
			this->rootNumber = vec[0];
			this->pagesCounter = vec[1];
		}
	}

	~TreePageManager() 
	{
		this->randomAccessFile.WriteRecords<std::size_t>({ this->rootNumber, this->pagesCounter }, 0, std::ios::beg);
	}

	TreePage* ReadPageWithCache(std::size_t pageNumber);
	TreePage ReadPage(std::size_t pageNumber, bool incrementReadCounter);
	bool WritePage(const TreePage& treePage);
	bool FlushPageCache();

	TreePage CreateNewPage();
	std::size_t FindParentNumber(std::size_t childNumber);

	const std::size_t GetRootNumber() const;
	const std::size_t GetPagesReadCounter() const;
	const std::size_t GetPagesWrittenCounter() const;

	const void SetRootNumber(std::size_t rootNumber);
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	std::unordered_map<std::size_t, TreePage> pageCache;
	std::vector<std::size_t> pageCacheOrder;
	std::size_t rootNumber;
	std::size_t pageSize;
	std::size_t pageRecordsNumber;
	std::size_t paramsNumber;
	std::size_t pagesCounter;
	std::size_t pagesReadCounter = 0;
	std::size_t pagesWrittenCounter = 0;
};


