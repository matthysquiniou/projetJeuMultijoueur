#pragma once
#include "Packet.h"

struct PacketManagerData;

enum class PacketStatus
{
	Acknowledged,
	Expired
};

struct PacketToSend : public Packet
{
	uint32_t packetID;
	bool replicate;
	uint8_t maxReplicateCount;
	uint8_t replicateCount = 0;
	std::function<void(PacketToSend&, PacketStatus, PacketManagerData&)> onComplete;
	float replicateBaseTimer = 0.033f; // allow resend once 1/30 seconds
	float replicateTimer = 0.033f; // send the first directly
	float replicateTimeout = 0.f;
	float replicateBaseTimeout = 2.f;
};