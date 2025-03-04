#include "DiskRecord.h"

void DiskRecord::Print() const
{
    std::cout << "Id: " << this->id << " ";
    std::cout << "  Side A: " << this->sideA << " ";
    std::cout << "  Side B: " << this->sideB << " ";
    std::cout << "  Angle: " << this->angleAlpha << "\n";
}

std::size_t DiskRecord::GetId() const
{
    return this->id;
}

std::size_t DiskRecord::GenerateRandomId()
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<std::size_t> distribution(1, 100000);
    std::size_t randomId = distribution(generator) % 100000;
    return randomId;
}
