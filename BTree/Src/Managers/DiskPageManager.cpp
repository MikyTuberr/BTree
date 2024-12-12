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

void DiskPageManager::RemoveRecordById(std::size_t pageNumber, std::size_t recordId)
{
	if (pageNumber != this->pagesCounter) {
		DiskPage page = this->ReadPage(pageNumber);
		page.RemoveRecordById(recordId);
		this->WritePage(page);
	}
	else {
		this->bufferPage.RemoveRecordById(recordId);
	}
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

void DiskPageManager::PrintFile()
{
    for (std::size_t pageIndex = 0; pageIndex < this->pagesCounter; ++pageIndex) {
        DiskPage page = this->ReadPage(pageIndex);
		this->pagesReadCounter--;
        const auto& records = page.GetRecords();

        std::cout << "Page " << pageIndex << ":\n";
        for (const auto& record : records) {
			record.Print();
        }
    }

    std::cout << "Buffer Page:\n";
    const auto& bufferRecords = this->bufferPage.GetRecords();
    for (const auto& record : bufferRecords) {
		record.Print();
    }
}

const std::streampos DiskPageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return (pageNumber * this->bufferPage.GetPageSize()) + DISK_FILE_OFFSET;
}

bool DiskPageManager::WriteBuffer()
{
	std::streampos cursor = this->CalculateCursor(this->pagesCounter);
	return this->randomAccessFile.WriteRecords(this->bufferPage.GetRecords(), cursor, std::ios_base::beg);
}
