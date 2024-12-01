#pragma once
#include "./Records/DiskRecord.h"
#include "./Pages/DiskPage.h"
#include <iostream>
#include <fstream>
#include <vector>

class RandomAccessFile
{
public:
	RandomAccessFile(const std::string& fileName, const std::ios_base::openmode& openmode)
		: file(fileName, openmode)
	{
		if (!file.is_open()) {
			throw std::runtime_error("Unable to open file: " + fileName);
		}
	}

	template <typename Record>
	std::vector<Record> ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction,
		const std::size_t& numberOfRecordsToRead);

	template <typename Record, typename Param>
	std::pair<std::vector<Record>, std::vector<Param>> ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction,
		const std::size_t& numberOfRecordsToRead, const std::size_t& paramsToReadSize);

	template <typename Record>
	bool WriteRecords(const std::vector<Record>& records, const std::streampos& cursor, const std::ios_base::seekdir& direction);

	template <typename Record, typename Param>
	bool WriteRecords(const std::vector<Record>& records, const std::streampos& cursor, const std::ios_base::seekdir& direction,
		const std::vector<Param>& params);

private:
	std::fstream file;
};

template<typename Record>
inline std::vector<Record> RandomAccessFile::ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction,
	const std::size_t& numberOfRecordsToRead)
{
	file.seekg(cursor, direction);

	std::vector<Record> records;

	for (std::size_t i = 0; i < numberOfRecordsToRead; ++i) {
		Record record;
		file.read(reinterpret_cast<char*>(&record), sizeof(record));
		if (file.eof()) {
			std::cout << "EOF DETECTED! \n";
			return std::vector<Record>();
		}
		records.push_back(std::move(record));
	}

	return records;
}

template<typename Record, typename Param>
inline std::pair<std::vector<Record>, std::vector<Param>> RandomAccessFile::ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction, 
	const std::size_t& numberOfRecordsToRead, const std::size_t& numberOfParamsToRead)
{
	file.seekg(cursor, direction);

	std::vector<Param> params;
	for (std::size_t i = 0; i < numberOfParamsToRead; ++i) {
		Param paramRead;
		file.read(reinterpret_cast<char*>(&paramRead), sizeof(Param));

		if (file.eof()) {
			std::cerr << "EOF detected while reading parameters!\n";
			return std::make_pair(std::vector<Record>(), std::vector<Param>());
		}

		std::cout << "Odczytano parametr: " << paramRead << std::endl;
	}

	std::vector<Record> records;
	for (std::size_t i = 0; i < numberOfRecordsToRead; ++i) {
		Record record;
		file.read(reinterpret_cast<char*>(&record), sizeof(Record));

		if (file.eof()) {
			std::cerr << "EOF detected while reading records!\n";
			return std::make_pair(std::vector<Record>(), std::vector<Param>());
		}

		records.push_back(std::move(record));
	}

	return std::make_pair(records, params);
}

template<typename Record>
inline bool RandomAccessFile::WriteRecords(const std::vector<Record>& records, const std::streampos& cursor, const std::ios_base::seekdir& direction)
{
	return this->WriteRecords(records, cursor, direction, std::vector<std::size_t>());
}

template <typename Record, typename Param>
inline bool RandomAccessFile::WriteRecords(const std::vector<Record>& records, const std::streampos& cursor, const std::ios_base::seekdir& direction,
	const std::vector<Param>& params)
{
	file.seekp(cursor, direction);

	for (const auto& param : params) {
		file.write(reinterpret_cast<const char*>(&param), sizeof(param));
		if (!file) {
			return false;
		}
	}

	for (const auto& record : records) {
		file.write(reinterpret_cast<const char*>(&record), sizeof(record));
		if (!file) {
			return false;
		}
	}

	return true;
}
