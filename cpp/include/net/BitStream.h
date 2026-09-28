#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

class BitStream {
public:
    BitStream();
    explicit BitStream(size_t initialBytesToAllocate);
    BitStream(uint8_t* data, size_t sizeInBytes, bool copyData);
    ~BitStream();

    void Reset();
    void WriteBits(const uint8_t* input, size_t numberOfBitsToWrite);
    void ReadBits(uint8_t* output, size_t numberOfBitsToRead);

    template <typename T>
    void Write(T value) {
        WriteBits(reinterpret_cast<const uint8_t*>(&value), sizeof(T) * 8);
    }

    template <typename T>
    void Read(T& value) {
        ReadBits(reinterpret_cast<uint8_t*>(&value), sizeof(T) * 8);
    }

    void WriteString(const std::string& str);
    std::string ReadString(size_t length);

    uint8_t* GetData() const;
    size_t GetNumberOfBytesUsed() const;
    size_t GetNumberOfBitsUsed() const;
    size_t GetReadOffset() const;
    void SetReadOffset(size_t offset);

private:
    uint8_t* m_data;
    size_t m_numberOfBitsAllocated;
    size_t m_numberOfBitsUsed;
    size_t m_readOffset;
    bool m_copyData;

    void Reallocate(size_t newNumberOfBitsAllocated);
};
