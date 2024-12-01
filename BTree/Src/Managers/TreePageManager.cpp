#include "TreePageManager.h"

TreePage TreePageManager::ReadPage(std::size_t pageNumber)
{
	if (pageNumber > this->pagesCounter) {
		throw std::out_of_range("Page number " + std::to_string(pageNumber) + " does not exist.");
	}

	std::streampos cursor = this->CalculateCursor(pageNumber);

    // TODO:
    /*
    try {
        std::pair<std::vector<TreeRecord>, std::vector<std::size_t>> pair =
            this->randomAccessFile.ReadRecords<TreeRecord, std::size_t>(
                cursor, std::ios_base::beg, this->root.GetRecordsNumber(), this->paramsNumber);
    }
    catch ()
    {

    }*/

    std::pair<std::vector<TreeRecord>, std::vector<std::size_t>> pair =
        this->randomAccessFile.ReadRecords<TreeRecord, std::size_t>(
            cursor, std::ios_base::beg, this->root.GetRecordsNumber(), this->paramsNumber);

	std::vector<TreeRecord> records = pair.first;
	std::vector<std::size_t> params = pair.second;

	TreePage page = TreePage(this->root.GetPageSize(), this->root.GetRecordsNumber(), pageNumber, params[0], params[1], records);

	return page;
}

bool TreePageManager::WritePage(const TreePage& treePage)
{
    if (treePage.GetPageNumber() == this->pagesCounter) {
        this->pagesCounter++;
    }

    std::streampos cursor = this->CalculateCursor(treePage.GetPageNumber());
    std::vector<TreeRecord> fixedRecords = treePage.GetFixedRecords();
    return this->randomAccessFile.WriteRecords(fixedRecords, cursor, std::ios_base::beg, std::vector<std::size_t>(treePage.GetParentPageNumber(), treePage.GetHeadLeftChildPageNumber()));
}

TreePage TreePageManager::GetRoot() const
{
	return this->root;
}

bool TreePageManager::InsertRecord(TreeRecord treeRecord, TreePage treePage)
{
	// TODO:
    // MOØLIWE ØE TRZEBA TWORZY∆ PUSTE STRONY PO UTWORZENIU WSKAèNIKA

	std::vector<TreeRecord> records = treePage.GetRecords();

	auto it = std::upper_bound(records.begin(), records.end(), treeRecord,
		[](const TreeRecord& a, const TreeRecord& b) {
			return a.GetId() < b.GetId();
		});

	TreeRecord* leftNeighbour = (it != records.begin()) ? &*(it - 1) : nullptr;
	TreeRecord* rightNeighbour = (it != records.end()) ? &*it : nullptr;

    if (leftNeighbour && rightNeighbour) {
        std::size_t leftNeigbourChildNumber = leftNeighbour->GetTreeRightChildNumber();
        std::size_t rightNeigbourChildNumber = rightNeighbour->GetTreeRightChildNumber();

        if (leftNeigbourChildNumber == rightNeigbourChildNumber) {
            treeRecord.SetTreeRightChildNumber(leftNeigbourChildNumber);
        }
    }
    else if (leftNeighbour) {
        std::size_t rightNeigbourChildNumber = rightNeighbour->GetTreeRightChildNumber();
        TreePage treePage = this->ReadPage(rightNeigbourChildNumber);
        // TODO: maybe flush needed?
        this->pagesCounter++;
        treeRecord.SetTreeRightChildNumber(this->pagesCounter);
    }
    else if (rightNeighbour) {
        std::size_t leftNeigbourChildNumber = leftNeighbour->GetTreeRightChildNumber();
        TreePage treePage = this->ReadPage(leftNeigbourChildNumber);
        treeRecord.SetTreeRightChildNumber(treePage.GetHeadLeftChildPageNumber());
        // TODO: maybe flush needed?
        this->pagesCounter++;
        treePage.SetHeadLeftChildPageNumber(this->pagesCounter);
    }
    else {
        this->pagesCounter++;
        treePage.SetHeadLeftChildPageNumber(this->pagesCounter);
        this->pagesCounter++;
        treeRecord.SetTreeRightChildNumber(this->pagesCounter);
    }

    if (!treePage.InsertRecord(treeRecord)) {
        return false;
    }

    return this->WritePage(treePage);
}

TreeRecord* TreePageManager::FindRecordInPageById(TreePage& page, const std::size_t& id)
{
	return page.FindRecordById(id);
}

const std::streampos TreePageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return pageNumber * this->root.GetPageSize();
}
