#include "TreePageManager.h"

TreePage TreePageManager::ReadPage(std::size_t pageNumber)
{
	if (pageNumber > this->pagesCounter) {
		throw std::out_of_range("Page number " + std::to_string(pageNumber) + " does not exist.");
	}

	std::streampos cursor = this->CalculateCursor(pageNumber);

	std::pair<std::vector<TreeRecord>, std::vector<std::size_t>> pair =
		this->randomAccessFile.ReadRecords<TreeRecord, std::size_t>(
			cursor, std::ios_base::beg, this->root.GetRecordsNumber(), this->paramsNumber);

	std::vector<TreeRecord> records = pair.first;
	std::vector<std::size_t> params = pair.second;

	TreePage page = TreePage(this->root.GetPageSize(), this->root.GetRecordsNumber(), pageNumber, params[0], params[1], records);

	return page;
}

TreePage TreePageManager::GetRoot() const
{
	return this->root;
}

bool TreePageManager::InsertRecord(const TreeRecord treeRecord)
{
	// TODO
	//return this->InsertRecord(treeRecord);
}

TreeRecord* TreePageManager::FindRecordInPageById(const TreePage& page, const std::size_t& id) const
{
	return page.FindRecordById(id);
}

const std::streampos TreePageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return pageNumber * this->root.GetPageSize();
}
