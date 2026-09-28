#pragma once

#include "../net/BitStream.h"
#include "../net/RakClient.h"
#include "DialogManager.h"

class RPCManager {
public:
    explicit RPCManager(RakClient* client);
    void HandleRPC(BitStream& bs);

private:
    RakClient* m_client;
    DialogManager m_dialogManager;
};
