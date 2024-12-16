#include "RandomAccessFile.h"

bool RandomAccessFile::isFileEmpty()
{
    if (!file.is_open()) {
        std::cerr << "isFileEmpty: File is not open.\n";
        return true;
    }

    file.seekg(0, std::ios::end);
    if (!file.good()) {
        std::cerr << "isFileEmpty: Failed to seek to the end of the file.\n";
        return true;
    }

    std::streampos endPosition = file.tellg();
    if (endPosition == std::streampos(-1)) {
        std::cerr << "isFileEmpty: tellg() failed.\n";
        return true;
    }

    return endPosition == 0;
}

bool RandomAccessFile::readBuffer(std::vector<char>& buffer, size_t bytesToRead)
{
    if (!file.is_open()) {
        std::cerr << "readBuffer: File is not open.\n";
        return false;
    }

    if (bytesToRead == 0) {
        std::cerr << "readBuffer: No bytes requested to read.\n";
        return false;
    }

    buffer.resize(bytesToRead);
    file.read(buffer.data(), bytesToRead);

    if (!file.good()) {
        if (file.eof()) {
            std::cerr << "readBuffer: End of file reached before reading " << bytesToRead << " bytes.\n";
        }
        else {
            std::cerr << "readBuffer: Error occurred while reading the file.\n";
        }
        return false;
    }

    if (static_cast<size_t>(file.gcount()) != bytesToRead) {
        std::cerr << "readBuffer: Incomplete read. Expected " << bytesToRead
            << " bytes but got " << file.gcount() << " bytes.\n";
        return false;
    }

    return true;
}

bool RandomAccessFile::writeBuffer(const std::vector<char>& buffer)
{
    if (!file.is_open()) {
        std::cerr << "writeBuffer: File is not open.\n";
        return false;
    }

    if (buffer.empty()) {
        std::cerr << "writeBuffer: Buffer is empty, nothing to write.\n";
        return false;
    }

    file.write(buffer.data(), buffer.size());

    if (!file.good()) {
        std::cerr << "writeBuffer: Error occurred while writing to the file.\n";
        return false;
    }

    file.flush();
    if (!file.good()) {
        std::cerr << "writeBuffer: Error occurred during file flush.\n";
        return false;
    }

    return true;
}
