#include "TreePage.h"

bool TreePage::InsertRecord(const TreeRecord treeRecord)
{
    //if (this->isOverflow()) {
        //return false;
    //}

    auto it = std::lower_bound(records.begin(), records.end(), treeRecord,
        [](const TreeRecord& a, const TreeRecord& b) {
            return a.GetId() < b.GetId();
        });

    records.insert(it, treeRecord);

    return true;
}

TreeRecord TreePage::FindRecordById(std::size_t id)
{
    int left = 0;
    int right = int(records.size() - 1);

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        std::size_t midId = records[mid].GetId();

        if (midId == id)
        {
            return records[mid];
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

    return TreeRecord();
}

std::pair<std::size_t, TreeRecord> TreePage::FindRightSiblingNumberById(std::size_t id)
{
    for (std::size_t i = 0; i < this->records.size(); ++i) {
        if (id < this->records[i].GetId()) {
            std::size_t rightSiblingNumber = this->GetRightChildPageNumberById(i);
            TreeRecord separatorRecord = this->records[i]; // chyba git
            return { rightSiblingNumber, separatorRecord };
        }
    }

    std::size_t lastRightChildPageNumber = this->GetRightChildPageNumberById(this->records.size() - 1);
    TreeRecord lastSeparator = this->records.back();

    return { lastRightChildPageNumber, lastSeparator };
}

TreeRecord TreePage::PopTail()
{
    if (records.empty()) {
        throw std::out_of_range("Attempted to pop from an empty container");
    }

    TreeRecord tail = records.back();
    records.pop_back();

    return tail;
}

TreeRecord TreePage::PopHead()
{
    if (records.empty()) {
        throw std::out_of_range("Attempted to pop from an empty container");
    }

    TreeRecord head = records.front();
    records.erase(records.begin());

    return head;
}

bool TreePage::ReplaceRecord(TreeRecord recordToReplace, TreeRecord newRecord)
{
    for (auto it = records.begin(); it != records.end(); ++it) {
        if (it->GetId() == recordToReplace.GetId()) {
            *it = newRecord;

            if (it != records.begin() && (it - 1)->GetId() > newRecord.GetId()) {
                std::sort(records.begin(), records.end(), [](const TreeRecord& a, const TreeRecord& b) {
                    return a.GetId() < b.GetId();
                    });
            }
            else if (it + 1 != records.end() && (it + 1)->GetId() < newRecord.GetId()) {
                std::sort(records.begin(), records.end(), [](const TreeRecord& a, const TreeRecord& b) {
                    return a.GetId() < b.GetId();
                    });
            }

            return true;
        }
    }

    return false;
}

std::pair<std::size_t, TreeRecord> TreePage::FindLeftSiblingNumberById(std::size_t id)
{
    for (std::size_t i = 0; i < this->records.size(); ++i) {
        if (id < this->records[i].GetId()) {
            if (i > 0) {
                std::size_t leftSiblingNumber = this->GetRightChildPageNumberById(i-1);
                TreeRecord separatorRecord = this->records[i]; // CHYBA GIT, ALE POTENCJALNY PROBLEM MOZE BYC
                return { leftSiblingNumber, separatorRecord };
            }
            else {
                std::size_t leftSiblingNumber = this->GetHeadLeftChildPageNumber();
                TreeRecord separatorRecord = this->records[i];
                return { leftSiblingNumber, separatorRecord };
            }

        }
    }

    std::size_t headLeftChildPageNumber = this->GetHeadLeftChildPageNumber();
    TreeRecord separatorRecord = this->records.front();

    return { headLeftChildPageNumber, separatorRecord };
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
    if (index < records.size()) {
        return this->records[index].GetTreeRightChildNumber();
    }
    return NULLPTR;
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

const TreeRecord TreePage::GetHead() const
{
    if (records.size() > 0) {
        return this->records[0];
    }
    return TreeRecord();
}

const void TreePage::SetRecords(const std::vector<TreeRecord> treeRecords)
{
    this->records = treeRecords;
}

const void TreePage::SetHeadLeftChildPageNumber(const std::size_t treePageNumber)
{
    this->headLeftChildPageNumber = treePageNumber;
}

const void TreePage::SetParentPageNumber(const std::size_t parentPageNumber)
{
    this->parentPageNumber = parentPageNumber;
}

void TreePage::Print() const
{
    //std::cout << "  Tree page: ";
    //if (this->pageNumber == NULLPTR) {
        //std::cout << "NULL";
    //}
    //else {
       // std::cout << this->pageNumber;
    //}

    std::cout << "  Head left child: ";
    if (this->headLeftChildPageNumber == NULLPTR) {
        std::cout << "NULL";
    }
    else {
        std::cout << this->headLeftChildPageNumber;
    }

    std::cout << "  Parent: ";
    if (this->parentPageNumber == NULLPTR) {
        std::cout << "NULL";
    }
    else {
        std::cout << this->parentPageNumber;
    }

    std::cout << "\n  Content: \n";
    for (const auto& record : this->records) {
        record.Print();
    }
}
