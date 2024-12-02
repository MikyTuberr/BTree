#pragma once
#include "./Records/DiskRecord.h"
#include "./Records/TreeRecord.h"

constexpr std::size_t DISK_PAGE_SIZE = 320;
constexpr std::size_t TREE_PAGE_SIZE = 240;

constexpr std::size_t DISK_RECORD_SIZE = sizeof(DiskRecord);
constexpr std::size_t TREE_RECORD_SIZE = sizeof(TreeRecord);

constexpr std::size_t d = TREE_PAGE_SIZE / TREE_RECORD_SIZE;

//constexpr std::size_t MIN_TREE_PAGE_KEYS_NUMBER = d;
//constexpr std::size_t MAX_TREE_PAGE_KEYS_NUMBER = 2 * d;

constexpr std::size_t TREE_PAGE_PARAMS_NUMBER = 2;
constexpr std::size_t TREE_PAGE_PARAMS_SIZE = 2 * sizeof(std::size_t);