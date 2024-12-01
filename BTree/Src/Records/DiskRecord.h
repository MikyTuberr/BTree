#pragma once
#include <iostream>
#include <random>
#include <ctime>

class DiskRecord
{
public:
    DiskRecord(double a = 0, double b = 0, double alpha = 0, std::size_t key = 0)
        : sideA(a), sideB(b), angleAlpha(alpha), id(key)
    {
        if (id == 0) {
            id = GenerateRandomId();
        }
    }

    std::size_t GetId() const;

    void Print() const;
private:
    std::size_t GenerateRandomId();

    std::size_t id;
    double sideA;
    double sideB;
    double angleAlpha;
};