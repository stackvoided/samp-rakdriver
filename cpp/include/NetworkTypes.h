#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct PlayerID {
    uint32_t binaryAddress;
    uint16_t port;

    bool operator==(const PlayerID& other) const {
        return binaryAddress == other.binaryAddress && port == other.port;
    }

    bool operator!=(const PlayerID& other) const {
        return !(*this == other);
    }
};
#pragma pack(pop)

const PlayerID UNASSIGNED_PLAYER_ID = { 0xFFFFFFFF, 0xFFFF };
