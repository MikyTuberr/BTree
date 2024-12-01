#include "DiskRecord.h"

void DiskRecord::Print() const
{
    std::cout << "Disk record: " << this << "\n";
    std::cout << "  Id: " << this->id << "\n";
    std::cout << "  Side A: " << this->sideA << "\n";
    std::cout << "  Side B: " << this->sideB << "\n";
    std::cout << "  Angle Alpha: " << this->angleAlpha << "\n";
}

std::size_t DiskRecord::GetId() const
{
    return this->id;
}

std::size_t DiskRecord::GenerateRandomId()
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<std::size_t> distribution(1, std::numeric_limits<std::size_t>::max());
    return distribution(generator);
}
