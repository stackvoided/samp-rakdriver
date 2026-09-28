#pragma once

#include <cstdint>

enum RPCID : uint8_t {
    RPC_ServerJoin = 25,
    RPC_ServerQuit = 26,
    RPC_InitGame = 139,
    RPC_ClientJoin = 25,
    RPC_ShowDialog = 61,
    RPC_DialogResponse = 62,
    RPC_WorldPlayerAdd = 32,
    RPC_WorldPlayerRemove = 163
};
