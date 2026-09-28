#include "../../include/samp/DialogManager.h"
#include "../../include/core/Logger.h"
#include "../../include/net/PacketEnumerations.h"
#include "../../include/samp/SAMPDefines.h"

void DialogManager::HandleShowDialog(BitStream& bs) {
    DialogInfo dialog;
    bs.Read(dialog.id);
    bs.Read(dialog.style);

    uint8_t titleLen = 0;
    bs.Read(titleLen);
    dialog.title = bs.ReadString(titleLen);

    uint8_t b1Len = 0;
    bs.Read(b1Len);
    dialog.button1 = bs.ReadString(b1Len);

    uint8_t b2Len = 0;
    bs.Read(b2Len);
    dialog.button2 = bs.ReadString(b2Len);

    size_t textLen = (bs.GetNumberOfBytesUsed() * 8 - bs.GetReadOffset()) / 8;
    dialog.text = bs.ReadString(textLen);

    LOG_INFO("--- DIALOG INCOMING ---");
    LOG_INFO("ID: " + std::to_string(dialog.id) + " | Style: " + std::to_string(dialog.style));
    LOG_INFO("Title: " + dialog.title);
    LOG_INFO("Content: " + dialog.text);
    LOG_INFO("Buttons: [" + dialog.button1 + "] [" + dialog.button2 + "]");
}

BitStream DialogManager::BuildDialogResponse(uint16_t dialogId, uint8_t buttonId, uint16_t listItem, const std::string& inputText) {
    BitStream bs;
    bs.Write<uint8_t>(ID_RPC);
    bs.Write<uint8_t>(RPC_DialogResponse);
    bs.Write<uint16_t>(dialogId);
    bs.Write<uint8_t>(buttonId);
    bs.Write<uint16_t>(listItem);
    bs.Write<uint8_t>(static_cast<uint8_t>(inputText.length()));
    bs.WriteString(inputText);
    return bs;
}
