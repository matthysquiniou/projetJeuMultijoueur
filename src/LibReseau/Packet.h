#pragma once
#include <vector>
#include <WinSock2.h>

struct Packet
{
    std::vector<uint8_t> data; 
    sockaddr_in from;  
};