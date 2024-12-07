#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/TreePage.h"
#include <string>
#include "PageManagerConfig.h"
#include <unordered_map>

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

	//TODO: zapisywanie w pliku wskaznika na root przy wywo³aniu dekstruktora i wczytywanie przy konstruktorze!

	TreePage* ReadPageWithCache(std::size_t pageNumber);
	TreePage ReadPage(std::size_t pageNumber);
	bool WritePage(const TreePage& treePage);
	bool FlushPageCache();

	TreePage CreateNewPage();

	const std::size_t GetRootNumber() const;

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
	std::size_t pagesCounter = 0;
};


