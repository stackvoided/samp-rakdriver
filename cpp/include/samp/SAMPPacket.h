#pragma once

#include "../net/BitStream.h"
#include "../core/Config.h"

class SAMPPacket {
public:
    static BitStream BuildClientJoin(const BotConfig& config, uint16_t challenge);
};
