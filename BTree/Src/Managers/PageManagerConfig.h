#pragma once
#include <iostream>

struct PageManagerConfig
{
    PageManagerConfig(std::size_t pageSize, std::size_t pageRecordsNumber, std::string filename, 
        std::ios_base::openmode openmode)
    : pageSize(pageSize), pageRecordsNumber(pageRecordsNumber), filename(filename), openmode(openmode) {}

    std::size_t pageSize = 0;
    std::size_t pageRecordsNumber = 0;
    std::string filename = "";
    std::ios_base::openmode openmode = std::ios_base::openmode();
};