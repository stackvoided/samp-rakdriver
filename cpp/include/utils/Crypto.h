#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Crypto {
public:
    static void GenerateAuthKey(char* buffer, const char* authKey);
    static uint32_t CalculateCRC32(const uint8_t* data, size_t length);
};
