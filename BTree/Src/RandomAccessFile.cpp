#include "RandomAccessFile.h"

bool RandomAccessFile::isFileEmpty()
{
    file.seekg(0, std::ios::end);
    return file.tellg() == 0;
}

bool RandomAccessFile::readBuffer(std::vector<char>& buffer, size_t bytesToRead)
{
	buffer.resize(bytesToRead);
	file.read(buffer.data(), bytesToRead);
	return file.good() && static_cast<size_t>(file.gcount()) == bytesToRead;
}

bool RandomAccessFile::writeBuffer(const std::vector<char>& buffer)
{
    file.write(buffer.data(), buffer.size());
    return file.good();
}
