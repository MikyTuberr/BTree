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
			this->bufferRecordsCounter = 0;
		}
		else {
			auto vec = this->randomAccessFile.ReadRecords<std::size_t>(0, std::ios::beg, DISK_FILE_PARAMS_NUMBER);
			this->pagesCounter = vec[0];
			this->bufferRecordsCounter = vec[1];
			std::streampos cursor = this->CalculateCursor(this->pagesCounter);
			this->bufferPage.SetRecords(this->randomAccessFile.ReadRecords<DiskRecord>(cursor, std::ios::beg, this->bufferRecordsCounter));
		}
	}

	~DiskPageManager() 
	{
		this->bufferRecordsCounter = this->bufferPage.GetRecords().size();
		if (!this->WriteBuffer()) {
			std::cerr << "Error while writing DiskPageManager buffer\n";
		};
		this->randomAccessFile.WriteRecords<std::size_t>({ this->pagesCounter, this->bufferRecordsCounter }, 0, std::ios::beg);
	}

	DiskPage ReadPage(std::size_t pageNumber);
	bool WritePage(const DiskPage& diskPage);

	void UpdateRecordById(DiskRecord updatedRecord, std::size_t pageNumber, std::size_t recordId);
	void RemoveRecordById(std::size_t pageNumber, std::size_t recordId);
	void InsertRecordToBuffer(const DiskRecord diskRecord);
	DiskRecord* FindRecordInBufferById(const std::size_t& id);
	DiskRecord* FindRecordInPageById(DiskPage& page, const std::size_t& id);

	const std::size_t GetPagesCounter() const;
	const std::size_t GetPagesReadCounter() const;
	const std::size_t GetPagesWrittenCounter() const;

	void PrintFile();
private:
	const std::streampos CalculateCursor(const std::size_t& pageNumber) const;
	bool WriteBuffer();

	RandomAccessFile randomAccessFile;
	DiskPage bufferPage;
	std::size_t pagesCounter;
	std::size_t bufferRecordsCounter;
	std::size_t pagesReadCounter = 0;
	std::size_t pagesWrittenCounter = 0;
};