#pragma once
#include <PacketManagerData.h>

struct ClientAppData
{
	~ClientAppData() = default;
	PacketManagerData clientData;
	char name[16];
	bool isDead = false;
	float pingTime = 0.f;
};

