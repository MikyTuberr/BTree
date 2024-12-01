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
    std::cout << "    Id: ";
    if (this->diskRecordId == NULLPTR) {
        std::cout << "NULL";
    }
    else {
        std::cout << this->diskRecordId;
    }

    std::cout << "  Disk page number: ";
    if (this->diskRecordPageNumber == NULLPTR) {
        std::cout << "NULL";
    }
    else {
        std::cout << this->diskRecordPageNumber;
    }

    std::cout << "  Right child: ";
    if (this->treeRightChildNumber == NULLPTR) {
        std::cout << "NULL";
    }
    else {
        std::cout << this->treeRightChildNumber;
    }

    std::cout << "\n";
}
