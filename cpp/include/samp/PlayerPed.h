#pragma once

#include "SyncStructures.h"
#include "../net/RakClient.h"

class PlayerPed {
public:
    PlayerPed();
    void SetPosition(const Vector3& pos);
    Vector3 GetPosition() const;
    void SendOnFootSync(RakClient* client);

private:
    PlayerSyncData m_syncData;
};
