#include "../../include/samp/RPCManager.h"
#include "../../include/samp/SAMPDefines.h"

RPCManager::RPCManager(RakClient* client) : m_client(client) {}

void RPCManager::HandleRPC(BitStream& bs) {
    uint8_t rpcId;
    bs.Read(rpcId);

    switch (rpcId) {
        case RPC_ShowDialog:
            m_dialogManager.HandleShowDialog(bs);
            break;
        default:
            break;
    }
}
