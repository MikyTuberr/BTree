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
		randomAccessFile(config.filename, config.openmode) 
	{
		if (this->randomAccessFile.isFileEmpty()) {
			this->pagesCounter = 0;
			//this->randomAccessFile.WriteRecords<std::size_t>({ this->pagesCounter }, 0, std::ios::beg);
		}
		else {
			auto vec = this->randomAccessFile.ReadRecords<std::size_t>(0, std::ios::beg, DISK_FILE_PARAMS_NUMBER);
			this->pagesCounter = vec[0];
		}
	}

	~DiskPageManager() 
	{
		std::size_t recordsSize = this->bufferPage.GetRecords().size();
		if (recordsSize > 0) {
			this->WritePage(this->bufferPage);
		}
		this->randomAccessFile.WriteRecords<std::size_t>({ this->pagesCounter }, 0, std::ios::beg);
	}

	DiskPage ReadPage(std::size_t pageNumber);
	bool WritePage(const DiskPage& diskPage);

	void InsertRecordToBuffer(const DiskRecord diskRecord);
	DiskRecord* FindRecordInBufferById(const std::size_t& id);
	DiskRecord* FindRecordInPageById(DiskPage& page, const std::size_t& id);

	const std::size_t GetPagesCounter() const;
	const std::size_t GetPagesReadCounter() const;
	const std::size_t GetPagesWrittenCounter() const;
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;

	RandomAccessFile randomAccessFile;
	DiskPage bufferPage;
	std::size_t pagesCounter;
	// moze tez rekord counter zeby nie marnowac pamieci
	std::size_t pagesReadCounter = 0;
	std::size_t pagesWrittenCounter = 0;
};