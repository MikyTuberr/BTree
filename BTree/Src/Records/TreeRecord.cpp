#include "TreeRecord.h"

std::size_t TreeRecord::GetId() const
{
    return this->diskRecordId;
}

std::size_t TreeRecord::GetTreeRightChildNumber() const
{
    return this->treeRightChildNumber;
}

void TreeRecord::SetTreeRightChildNumber(const std::size_t childTreePageNumber)
{
    this->treeRightChildNumber = childTreePageNumber;
}

void TreeRecord::Print() const
{
    std::cout << "Tree record: \n";
    std::cout << "  Id: " << this->diskRecordId << "\n";
    std::cout << "  Disk record page number: " << this->diskRecordPageNumber << "\n";
    std::cout << "  Right child tree page number: " << this->treeRightChildNumber << "\n";
}
