#include "../../include/samp/SAMPPacket.h"
#include "../../include/net/PacketEnumerations.h"
#include "../../include/utils/Crypto.h"

BitStream SAMPPacket::BuildClientJoin(const BotConfig& config, uint16_t challenge) {
    BitStream bs;
    bs.Write<uint8_t>(ID_SAMP_PACKET);
    bs.Write<uint16_t>(config.netGameVersion);
    bs.Write<uint8_t>(1);

    char authBuffer[64]{ 0 };
    Crypto::GenerateAuthKey(authBuffer, config.authKey.c_str());

    bs.Write<uint8_t>(static_cast<uint8_t>(config.nickname.length()));
    bs.WriteString(config.nickname);
    bs.Write<uint32_t>(challenge);
    bs.Write<uint8_t>(static_cast<uint8_t>(std::strlen(authBuffer)));
    bs.WriteString(authBuffer);
    bs.Write<uint8_t>(static_cast<uint8_t>(config.clientVersion.length()));
    bs.WriteString(config.clientVersion);

    return bs;
}
