#pragma once
#include <cstdint>

enum class HeaderFlags
{
    NONE,
    ALWAYS_PROCESS
};

struct PacketHeader
{
    uint32_t id;
    uint32_t frameId;
    uint8_t flag;
    uint8_t payloadType;
    bool needACK;
};