#include "DiskPage.h"

const std::size_t DiskPage::GetPageSize() const
{
    return this->pageSize;
}

const std::size_t DiskPage::GetRecordsNumber() const
{
    return this->recordsNumber;
}

const std::size_t DiskPage::GetPageNumber() const
{
    return this->pageNumber;
}

const std::vector<DiskRecord> DiskPage::GetRecords() const
{
    return this->records;
}

const std::vector<DiskRecord> DiskPage::GetFixedRecords() const
{
    std::vector<DiskRecord> fixedRecords = this->records;

    while (fixedRecords.size() < recordsNumber) {
        fixedRecords.push_back(DiskRecord());
    }

    return fixedRecords;
}

const void DiskPage::SetRecords(const std::vector<DiskRecord> diskRecords)
{
    this->records = diskRecords;
}

bool DiskPage::InsertRecord(const DiskRecord diskRecord)
{
    if (!this->isOverflow()) {
        this->records.push_back(diskRecord);
        return true;
    }
    return false;
}

DiskRecord* DiskPage::FindRecordById(std::size_t id)
{
    int left = 0;
    int right = int(records.size() - 1);

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
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

void DiskPage::RemoveRecordById(std::size_t id)
{
    auto& records = this->records;

    auto newEnd = std::remove_if(records.begin(), records.end(), [id](const DiskRecord& record) {
        return record.GetId() == id;
        });

    records.erase(newEnd, records.end());
}

bool DiskPage::isOverflow() const
{
    return this->records.size() == this->recordsNumber;
}

void DiskPage::Print() const
{
    std::cout << "Disk page: " << this->pageNumber << " contents:\n";
    for (const auto& record : this->records) {
        record.Print();
    }
}
