#pragma once
#include "NetworkEntityData.h"
#include "CollisionType.h"
// payload must be under 1024 bytes

enum class PayloadType : uint8_t
{
	CONNECT,
	PING,
	ACK,
	NAME,
	COMFIRM_NAME,
	ENTITY_INFO,
	GIVE_ENTITY_NETWORK_ID,
	COLLIDE,
	CHAT,
	PLAYER_DEATH,
	GAME_TIMER
};

// payload cote reseau

struct PayloadConnect
{
};

struct PayloadPing
{
};

struct PayloadACK
{
	uint32_t packetToACKID;
};

// payload cote app

struct PayloadName
{
	char name[16]{};
};

struct PayloadComfirmName
{
	bool isNameValid;
};

struct PayloadEntityInfo
{
	NetworkEntityData entityData;
};

struct PayloadGiveEntityNetworkId
{
	uint32_t localId;
	uint32_t networkId;
};

struct PayloadCollide
{
	uint32_t networkId;
	uint32_t collidedWithNetworkId;
	CollisionType collisionType;
};

struct PayloadChat
{
	char message[32]{};
};

struct PayloadPlayerDeath
{
	uint32_t networkId;
	uint32_t killedByNetworkId;
};

struct PayloadGameTimer
{
	float time;
};