#pragma once
#include "PacketToSend.h"
#include <vector>
#include <unordered_set>

struct PacketManagerData
{
    uint32_t lastIDReceived = 0;
    uint32_t lastIDSent = 0;
    uint32_t lastFrameIDSent = 0;
    uint32_t lastFrameIDReceived = 0;
    sockaddr_in addr;
    std::vector<PacketToSend> toSend;
    std::unordered_set<uint32_t> alwaysProccedIds;
};
