#pragma once
#include <iostream>
#include <random>
#include <ctime>
#include "../Type.h"

class DiskRecord
{
public:
    DiskRecord()
        : sideA(0), sideB(0), angleAlpha(0), id(0) {}

    DiskRecord(double a, double b, double alpha)
        : sideA(a), sideB(b), angleAlpha(alpha)
    {
        id = GenerateRandomId();
    }

    DiskRecord(double a, double b, double alpha, std::size_t key)
        : sideA(a), sideB(b), angleAlpha(alpha), id(key) {}

    std::size_t GetId() const;

    void Print() const;
private:
    std::size_t GenerateRandomId();

    std::size_t id;
    double sideA;
    double sideB;
    double angleAlpha;
};