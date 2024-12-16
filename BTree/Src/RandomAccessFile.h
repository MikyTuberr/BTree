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

	bool isFileEmpty();

private:
	bool readBuffer(std::vector<char>& buffer, size_t bytesToRead);
	bool writeBuffer(const std::vector<char>& buffer);

	std::fstream file;
};


template<typename Record>
inline std::vector<Record> RandomAccessFile::ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction,
	const std::size_t& numberOfRecordsToRead)
{
	if (numberOfRecordsToRead == 0) {
		std::cerr << "ReadRecords: No records to read.\n";
		return {};
	}

	file.seekg(cursor, direction);
	if (!file.good()) {
		std::cerr << "ReadRecords: Failed to seek to the desired position.\n";
		return {};
	}

	std::vector<Record> records(numberOfRecordsToRead);
	std::vector<char> buffer(numberOfRecordsToRead * sizeof(Record));

	if (!this->readBuffer(buffer, buffer.size())) {
		std::cerr << "ReadRecords: Error while reading records!\n";
		return {};
	}

	if (buffer.size() != numberOfRecordsToRead * sizeof(Record)) {
		std::cerr << "ReadRecords: Buffer size mismatch!\n";
		return {};
	}

	std::memcpy(records.data(), buffer.data(), buffer.size());
	return records;
}

template<typename Record, typename Param>
inline std::pair<std::vector<Record>, std::vector<Param>> RandomAccessFile::ReadRecords(const std::streampos& cursor, const std::ios_base::seekdir& direction,
	const std::size_t& numberOfRecordsToRead, const std::size_t& numberOfParamsToRead)
{
	if (numberOfRecordsToRead == 0 && numberOfParamsToRead == 0) {
		std::cerr << "ReadRecords: No records or parameters to read.\n";
		return { {}, {} };
	}

	file.seekg(cursor, direction);
	if (!file.good()) {
		std::cerr << "ReadRecords: Failed to seek to the desired position.\n";
		return { {}, {} };
	}

	std::vector<Param> params(numberOfParamsToRead);
	std::vector<char> paramBuffer(numberOfParamsToRead * sizeof(Param));

	if (!this->readBuffer(paramBuffer, paramBuffer.size())) {
		std::cerr << "ReadRecords: Error while reading parameters!\n";
		return { {}, {} };
	}

	std::memcpy(params.data(), paramBuffer.data(), paramBuffer.size());

	std::vector<Record> records(numberOfRecordsToRead);
	std::vector<char> recordBuffer(numberOfRecordsToRead * sizeof(Record));

	if (!this->readBuffer(recordBuffer, recordBuffer.size())) {
		std::cerr << "ReadRecords: Error while reading records!\n";
		return { {}, {} };
	}

	std::memcpy(records.data(), recordBuffer.data(), recordBuffer.size());

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
	if (records.empty() && params.empty()) {
		std::cerr << "WriteRecords: No records or parameters to write.\n";
		return false;
	}

	file.seekp(cursor, direction);
	if (!file.good()) {
		std::cerr << "WriteRecords: Failed to seek to the desired position.\n";
		return false;
	}

	if (!params.empty()) {
		std::vector<char> paramBuffer(params.size() * sizeof(Param));
		std::memcpy(paramBuffer.data(), params.data(), paramBuffer.size());

		if (!this->writeBuffer(paramBuffer)) {
			std::cerr << "WriteRecords: Error while writing parameters!\n";
			return false;
		}
	}

	if (!records.empty()) {
		std::vector<char> recordBuffer(records.size() * sizeof(Record));
		std::memcpy(recordBuffer.data(), records.data(), recordBuffer.size());

		if (!this->writeBuffer(recordBuffer)) {
			std::cerr << "WriteRecords: Error while writing records!\n";
			return false;
		}
	}

	return true;
}
