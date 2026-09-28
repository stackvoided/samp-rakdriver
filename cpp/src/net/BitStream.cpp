#include "../../include/net/BitStream.h"
#include <algorithm>

BitStream::BitStream() : m_data(nullptr), m_numberOfBitsAllocated(0), m_numberOfBitsUsed(0), m_readOffset(0), m_copyData(true) {
    Reallocate(256 * 8);
}

BitStream::BitStream(size_t initialBytesToAllocate) : m_data(nullptr), m_numberOfBitsAllocated(0), m_numberOfBitsUsed(0), m_readOffset(0), m_copyData(true) {
    Reallocate(initialBytesToAllocate * 8);
}

BitStream::BitStream(uint8_t* data, size_t sizeInBytes, bool copyData) : m_data(nullptr), m_numberOfBitsAllocated(sizeInBytes * 8), m_numberOfBitsUsed(sizeInBytes * 8), m_readOffset(0), m_copyData(copyData) {
    if (copyData) {
        m_data = new uint8_t[sizeInBytes];
        std::memcpy(m_data, data, sizeInBytes);
    } else {
        m_data = data;
    }
}

BitStream::~BitStream() {
    if (m_copyData && m_data) {
        delete[] m_data;
    }
}

void BitStream::Reset() {
    m_numberOfBitsUsed = 0;
    m_readOffset = 0;
}

void BitStream::Reallocate(size_t newNumberOfBitsAllocated) {
    if (!m_copyData) return;
    size_t newBytes = (newNumberOfBitsAllocated + 7) >> 3;
    uint8_t* newData = new uint8_t[newBytes]{};
    if (m_data) {
        size_t oldBytes = (m_numberOfBitsUsed + 7) >> 3;
        std::memcpy(newData, m_data, oldBytes);
        delete[] m_data;
    }
    m_data = newData;
    m_numberOfBitsAllocated = newNumberOfBitsAllocated;
}

void BitStream::WriteBits(const uint8_t* input, size_t numberOfBitsToWrite) {
    if (numberOfBitsToWrite == 0) return;
    if (m_numberOfBitsUsed + numberOfBitsToWrite > m_numberOfBitsAllocated) {
        Reallocate(std::max(m_numberOfBitsAllocated * 2, m_numberOfBitsUsed + numberOfBitsToWrite));
    }

    size_t writeOffset = m_numberOfBitsUsed;
    size_t byteOffset = writeOffset >> 3;
    size_t bitOffset = writeOffset & 7;

    while (numberOfBitsToWrite > 0) {
        size_t bitsThisByte = std::min(8 - bitOffset, numberOfBitsToWrite);
        uint8_t mask = (0xFF >> (8 - bitsThisByte)) << bitOffset;
        m_data[byteOffset] = (m_data[byteOffset] & ~mask) | ((*input << bitOffset) & mask);

        numberOfBitsToWrite -= bitsThisByte;
        writeOffset += bitsThisByte;
        byteOffset = writeOffset >> 3;
        bitOffset = writeOffset & 7;
        input++;
    }
    m_numberOfBitsUsed = writeOffset;
}

void BitStream::ReadBits(uint8_t* output, size_t numberOfBitsToRead) {
    if (numberOfBitsToRead == 0 || m_readOffset + numberOfBitsToRead > m_numberOfBitsUsed) return;

    size_t byteOffset = m_readOffset >> 3;
    size_t bitOffset = m_readOffset & 7;

    while (numberOfBitsToRead > 0) {
        size_t bitsThisByte = std::min(8 - bitOffset, numberOfBitsToRead);
        uint8_t mask = (0xFF >> (8 - bitsThisByte)) << bitOffset;
        *output = (m_data[byteOffset] & mask) >> bitOffset;

        numberOfBitsToRead -= bitsThisByte;
        m_readOffset += bitsThisByte;
        byteOffset = m_readOffset >> 3;
        bitOffset = m_readOffset & 7;
        output++;
    }
}

void BitStream::WriteString(const std::string& str) {
    uint32_t len = static_cast<uint32_t>(str.length());
    Write(len);
    WriteBits(reinterpret_cast<const uint8_t*>(str.c_str()), len * 8);
}

std::string BitStream::ReadString(size_t length) {
    if (length == 0) return "";
    std::string str(length, '\0');
    ReadBits(reinterpret_cast<uint8_t*>(&str[0]), length * 8);
    return str;
}

uint8_t* BitStream::GetData() const { return m_data; }
size_t BitStream::GetNumberOfBytesUsed() const { return (m_numberOfBitsUsed + 7) >> 3; }
size_t BitStream::GetNumberOfBitsUsed() const { return m_numberOfBitsUsed; }
size_t BitStream::GetReadOffset() const { return m_readOffset; }
void BitStream::SetReadOffset(size_t offset) { m_readOffset = offset; }
