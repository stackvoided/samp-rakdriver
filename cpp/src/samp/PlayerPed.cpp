#include "../../include/samp/PlayerPed.h"
#include "../../include/net/PacketEnumerations.h"

PlayerPed::PlayerPed() = default;

void PlayerPed::SetPosition(const Vector3& pos) {
    m_syncData.position = pos;
}

Vector3 PlayerPed::GetPosition() const {
    return m_syncData.position;
}

void PlayerPed::SendOnFootSync(RakClient* client) {
    if (!client || !client->IsConnected()) return;

    BitStream bs;
    bs.Write<uint8_t>(ID_PLAYER_SYNC);
    bs.WriteBits(reinterpret_cast<const uint8_t*>(&m_syncData), sizeof(PlayerSyncData) * 8);

    client->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}
