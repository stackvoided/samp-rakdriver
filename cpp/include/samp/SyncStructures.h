#pragma once

#include "../utils/Vector3.h"
#include <cstdint>

#pragma pack(push, 1)
struct PlayerSyncData {
    uint16_t lrAnalogKey{ 0 };
    uint16_t udAnalogKey{ 0 };
    uint16_t keys{ 0 };
    Vector3 position{};
    float quaternion[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
    uint8_t health{ 100 };
    uint8_t armour{ 0 };
    uint8_t weapon{ 0 };
    uint8_t specialAction{ 0 };
    Vector3 velocity{};
    Vector3 surfingOffsets{};
    uint16_t surfingVehicleId{ 0 };
    uint32_t animationId{ 0 };
};
#pragma pack(pop)
