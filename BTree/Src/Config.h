#pragma once
#include "./Records/DiskRecord.h"
#include "./Records/TreeRecord.h"

constexpr std::size_t d = 2;

constexpr std::size_t DISK_RECORD_SIZE = sizeof(DiskRecord);
constexpr std::size_t TREE_RECORD_SIZE = sizeof(TreeRecord);

constexpr std::size_t DISK_PAGE_SIZE = 2 * d * DISK_RECORD_SIZE;
constexpr std::size_t TREE_PAGE_SIZE = 2 * d * TREE_RECORD_SIZE;

constexpr std::size_t TREE_PAGE_PARAMS_NUMBER = 1;
constexpr std::size_t TREE_PAGE_PARAMS_SIZE = TREE_PAGE_PARAMS_NUMBER * sizeof(std::size_t);

constexpr std::size_t TREE_FILE_PARAMS_NUMBER = 2;
constexpr std::size_t DISK_FILE_PARAMS_NUMBER = 2;

constexpr std::size_t TREE_FILE_OFFSET = TREE_FILE_PARAMS_NUMBER * sizeof(std::size_t);
constexpr std::size_t DISK_FILE_OFFSET = DISK_FILE_PARAMS_NUMBER * sizeof(std::size_t);