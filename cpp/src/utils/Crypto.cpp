#include "../../include/utils/Crypto.h"
#include <cstring>

void Crypto::GenerateAuthKey(char* buffer, const char* authKey) {
    size_t len = std::strlen(authKey);
    for (size_t i = 0; i < len; ++i) {
        buffer[i] = authKey[i] ^ 0x05;
    }
    buffer[len] = '\0';
}

uint32_t Crypto::CalculateCRC32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}
