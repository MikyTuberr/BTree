#pragma once
#include "../RandomAccessFile.h"
#include "../Pages/DiskPage.h"
#include "../Records/DiskRecord.h"
#include <iostream>
#include <vector>
#include <string>
#include "PageManagerConfig.h"

class DiskPageManager
{
public:
	DiskPageManager(PageManagerConfig config)
		: bufferPage(config.pageSize, config.pageRecordsNumber, this->pagesCounter), 
		randomAccessFile(config.filename, config.openmode) {}

	~DiskPageManager();

	DiskPage ReadPage(std::size_t pageNumber);
	bool WritePage(const DiskPage& diskPage);

	void InsertRecordToBuffer(const DiskRecord diskRecord);
	DiskRecord* FindRecordInBufferById(const std::size_t& id);
	DiskRecord* FindRecordInPageById(DiskPage& page, const std::size_t& id);

	const std::size_t GetPagesCounter() const;
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	DiskPage bufferPage;
	std::size_t pagesCounter = 0;
};