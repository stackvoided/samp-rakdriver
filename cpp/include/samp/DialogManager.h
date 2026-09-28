#pragma once

#include "../net/BitStream.h"
#include <cstdint>
#include <string>

struct DialogInfo {
    uint16_t id{ 0 };
    uint8_t style{ 0 };
    std::string title;
    std::string button1;
    std::string button2;
    std::string text;
};

class DialogManager {
public:
    void HandleShowDialog(BitStream& bs);
    BitStream BuildDialogResponse(uint16_t dialogId, uint8_t buttonId, uint16_t listItem, const std::string& inputText);
};
