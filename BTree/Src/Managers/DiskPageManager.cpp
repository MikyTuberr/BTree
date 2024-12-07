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

	this->pagesReadCounter++;

	return page;
}

bool DiskPageManager::WritePage(const DiskPage& diskPage)
{
	std::streampos cursor = this->CalculateCursor(diskPage.GetPageNumber());
	std::vector<DiskRecord> fixedRecords = diskPage.GetFixedRecords();
	if (this->randomAccessFile.WriteRecords(fixedRecords, cursor, std::ios_base::beg)) {
		this->pagesWrittenCounter++;
		return true;
	}
	return false;
}

void DiskPageManager::InsertRecordToBuffer(const DiskRecord diskRecord)
{
	this->bufferPage.InsertRecord(diskRecord);
	if (this->bufferPage.isOverflow()) {
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

const std::size_t DiskPageManager::GetPagesCounter() const
{
	return this->pagesCounter;
}

const std::size_t DiskPageManager::GetPagesReadCounter() const
{
	return this->pagesReadCounter;
}

const std::size_t DiskPageManager::GetPagesWrittenCounter() const
{
	return this->pagesWrittenCounter;
}

const std::streampos DiskPageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return (pageNumber * this->bufferPage.GetPageSize()) + DISK_FILE_OFFSET;
}
