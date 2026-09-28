#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct PlayerID {
    uint32_t binaryAddress{ 0xFFFFFFFF };
    uint16_t port{ 0xFFFF };

    bool operator==(const PlayerID& other) const {
        return binaryAddress == other.binaryAddress && port == other.port;
    }
    bool operator!=(const PlayerID& other) const {
        return !(*this == other);
    }
};

struct Packet {
    PlayerID playerID;
    uint32_t length;
    uint32_t bitSize;
    uint8_t* data;
};
#pragma pack(pop)
