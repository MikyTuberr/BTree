#include "TreePage.h"

bool TreePage::InsertRecord(const TreeRecord treeRecord)
{
    if (!this->isOverflow()) {
        this->records.push_back(treeRecord);
        return true;
    }
    return false;
}

TreeRecord* TreePage::FindRecordById(std::size_t id)
{
    std::size_t left = 0;
    std::size_t right = records.size() - 1;

    while (left <= right)
    {
        std::size_t mid = left + (right - left) / 2;
        std::size_t midId = records[mid].GetId();

        if (midId == id)
        {
            return &records[mid];
        }
        else if (midId < id)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return nullptr;
}

bool TreePage::isOverflow() const
{
    return this->records.size() == this->recordsNumber;
}

const std::size_t TreePage::GetPageSize() const
{
    return this->pageSize;
}

const std::size_t TreePage::GetRecordsNumber() const
{
    return this->recordsNumber;
}

const std::size_t TreePage::GetPageNumber() const
{
    return this->pageNumber;
}

const std::size_t TreePage::GetParentPageNumber() const
{
    return this->parentPageNumber;
}

const std::size_t TreePage::GetRightChildPageNumberById(std::size_t index)
{
    return this->records[index].GetTreeRightChildNumber();
}

const std::size_t TreePage::GetHeadLeftChildPageNumber() const
{
    return this->headLeftChildPageNumber;
}

const std::vector<TreeRecord> TreePage::GetRecords() const
{
    return this->records;
}

const std::vector<TreeRecord> TreePage::GetFixedRecords() const
{
    std::vector<TreeRecord> fixedRecords = this->records;

    while (fixedRecords.size() < recordsNumber) {
        fixedRecords.push_back(TreeRecord());
    }

    return fixedRecords;
}

const void TreePage::SetRecords(const std::vector<TreeRecord> treeRecords)
{
    this->records = treeRecords;
}

const void TreePage::SetHeadLeftChildPageNumber(const std::size_t treePageNumber)
{
    this->headLeftChildPageNumber = treePageNumber;
}

void TreePage::Print() const
{
    std::cout << "Tree page: " << this->pageNumber << "\n";
    std::cout << "  Parent: " << this->parentPageNumber << "\n";
    std::cout << "  Content: \n";
    for (const auto& record : this->records) {
        record.Print();
    }
}
