#include "DiskPageManager.h"

DiskPage DiskPageManager::ReadPage(std::size_t pageNumber)
{
	if (pageNumber == this->pagesCounter) {
		return this->bufferPage;
	}
	else if (pageNumber > this->pagesCounter) {
		throw std::out_of_range("Page number " + std::to_string(pageNumber) + " does not exist.");
	}

	std::streampos cursor = this->CalculateCursor(pageNumber);

	std::vector<DiskRecord> records = this->randomAccessFile.
		ReadRecords<DiskRecord>(cursor, std::ios_base::beg, this->bufferPage.GetRecordsNumber());

	DiskPage page = DiskPage(this->bufferPage.GetPageSize(), this->bufferPage.GetRecordsNumber(), 
		pageNumber, records);

	return page;
}

bool DiskPageManager::WritePage(const DiskPage& diskPage)
{
	if (diskPage.GetPageNumber() == this->pagesCounter) {
		this->pagesCounter++;
	}

	std::streampos cursor = this->CalculateCursor(diskPage.GetPageNumber());
	std::vector<DiskRecord> fixedRecords = diskPage.GetFixedRecords();
	return this->randomAccessFile.WriteRecords(fixedRecords, cursor, std::ios_base::beg);
}

void DiskPageManager::InsertRecordToBuffer(const DiskRecord diskRecord)
{
	if (!this->bufferPage.InsertRecord(diskRecord)) {
		this->WritePage(this->bufferPage);
		this->pagesCounter++;
		this->bufferPage = DiskPage(this->bufferPage.GetPageSize(), 
			this->bufferPage.GetRecordsNumber(), this->pagesCounter);
	}
}

DiskRecord* DiskPageManager::FindRecordInBufferById(const std::size_t& id)
{
	return this->bufferPage.FindRecordById(id);
}

DiskRecord* DiskPageManager::FindRecordInPageById(DiskPage& page, const std::size_t& id)
{
	return page.FindRecordById(id);
}

const std::streampos DiskPageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return pageNumber * this->bufferPage.GetPageSize();
}
