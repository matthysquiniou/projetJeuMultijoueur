#pragma once
#include "MultiEntity.h"
#include "ColliderManager.h"
#include "MapManager.h"
#include <Payloads.h>

struct PacketManagerData;

class GameManager
{
public:
	GameManager();
	~GameManager();
	void Update();

	void HandlePayloadEntityInfo(PayloadEntityInfo payload, PacketManagerData* endpointData);

	void RemoveClient(PacketManagerData* endpointData);

	void AddEntity(MultiEntity* entity, PacketManagerData* endpointData = nullptr);
	void RemoveEntity(MultiEntity* entity, PacketManagerData* endpointData);

	void NewPlayerProtocol(PacketManagerData* endpointData);
	void DeletePlayerProtocol(PayloadPlayerDeath payload, PacketManagerData* endpointData);

	void HandleCollision(std::unordered_map<MultiEntity*, std::unordered_set<MultiEntity*>>& collisions);

private:
	void SendGameTimer();

	void FindPlayerSpawn(MultiEntity* entity);

	void CommunicateNewPositions();
	void AddEntity(PayloadEntityInfo payload, PacketManagerData* endpointData);
	void UpdateEntity(PayloadEntityInfo payload, PacketManagerData* endpointData);
	void RemoveEntity(PayloadEntityInfo payload, PacketManagerData* endpointData);

	void SendCollide(MultiEntity* entity, MultiEntity* CollidedWithEntity, CollisionType collisionType);

	void ClearDeadEntities();

	void Reset();

private:
	float m_baseGameTimer = 7200.f;
	float m_currentGameTimer = 7199.9f;

	float m_noPlayerSpawnRadius = 15.f;
	std::unordered_map<PacketManagerData*, std::unordered_set<MultiEntity*>> m_mapEndpointEntities;
	std::unordered_map<MultiEntity*, PacketManagerData*> m_mapEntitiesEndpoints;
	std::unordered_map<MultiEntityType, std::unordered_set<MultiEntity*>> m_mapMultiEntityTypeEntities;
	std::unordered_map<uint32_t, MultiEntity*> m_mapNetworkIdEntities;
	std::unordered_map<MultiEntity*, uint32_t> m_mapEntitiesNetworkId;

	std::unordered_set<MultiEntity*> m_deadEntities;

	ColliderManager m_colliderManager;
	MapManager m_mapManager;

	std::mt19937 m_randomGen;
	std::uniform_real_distribution<> m_mapPositionDistribution;
};

