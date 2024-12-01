#pragma once
#include <iostream>

struct PageManagerConfig
{
    std::string filename = "";
    std::ios_base::openmode openmode = std::ios_base::openmode();
    std::size_t pageSize = 0;
    std::size_t pageNumber = 0;
    std::size_t pageRecordsNumber = 0;
};