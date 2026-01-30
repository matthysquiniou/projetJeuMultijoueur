#pragma once
#include <cstdint>
#include "PacketHeader.h"

template<typename PayloadData>
struct PacketData
{
    PacketHeader header;
    PayloadData data;
};