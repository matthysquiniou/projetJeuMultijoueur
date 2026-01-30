#include "GameManager.h"
#include "NetworkIdManager.h"
#include "Server.h"
#include "App.h"

GameManager::GameManager()
{
	cpuEngine.EnableRender(false);
	m_randomGen = std::mt19937(std::random_device()());
	m_mapPositionDistribution = std::uniform_real_distribution<>(-100.f, 100.f);
}

GameManager::~GameManager()
{
	for (auto& it : m_mapNetworkIdEntities)
	{
		MultiEntity* entity = it.second;
		delete entity;
	}
	NetworkIdManager::ResetIds();
}

void GameManager::Update()
{
	m_currentGameTimer += cpuTime.delta;
	if (m_currentGameTimer >= m_baseGameTimer)
	{
		Reset();
		return;
	}

	ClearDeadEntities();
	CommunicateNewPositions();
	m_colliderManager.CheckCollision(m_mapMultiEntityTypeEntities, m_mapEntitiesEndpoints);
}

void GameManager::HandlePayloadEntityInfo(PayloadEntityInfo payload, PacketManagerData* endpointData)
{
	if (!payload.entityData.networkIdInit)
	{
		AddEntity(payload, endpointData);
		return;
	}

	if (!m_mapNetworkIdEntities.contains(payload.entityData.networkId))
		return;

	if (payload.entityData.deleteFlag)
	{
		RemoveEntity(payload, endpointData);
	}

	UpdateEntity(payload, endpointData);
}

void GameManager::RemoveClient(PacketManagerData* endpointData)
{
	auto it = m_mapEndpointEntities.find(endpointData);
	if (it == m_mapEndpointEntities.end())
		return;
	std::unordered_set<MultiEntity*> clientEntities = it->second;

	for (MultiEntity* entity : clientEntities)
	{
		RemoveEntity(entity, endpointData);
	}
}

void GameManager::AddEntity(MultiEntity* entity, PacketManagerData* endpointData)
{
	uint32_t id = NetworkIdManager::GetNewId();
	m_mapEndpointEntities[endpointData].insert(entity);
	m_mapNetworkIdEntities[id] = entity;
	m_mapEntitiesNetworkId[entity] = id;
	m_mapMultiEntityTypeEntities[entity->GetMultiEntityType()].insert(entity);
	m_mapEntitiesEndpoints[entity] = endpointData;
	PayloadEntityInfo payload;
	payload.entityData = entity->GetNetworkEntityData();
	payload.entityData.networkId = id;
	payload.entityData.networkIdInit = true;
	Server* server = App::GetInstance().GetServer();
	server->SendEntityInfo(payload,endpointData);
}

void GameManager::RemoveEntity(MultiEntity* entity, PacketManagerData* endpointData)
{
	uint32_t id = m_mapEntitiesNetworkId[entity];
	PayloadEntityInfo payload;
	payload.entityData = entity->GetNetworkEntityData();
	payload.entityData.networkId = id;
	payload.entityData.networkIdInit = true;
	payload.entityData.deleteFlag = true;
	Server* server = App::GetInstance().GetServer();
	server->SendEntityInfo(payload, endpointData,true);
	m_deadEntities.insert(entity);
}

void GameManager::NewPlayerProtocol(PacketManagerData* endpointData)
{
	MultiEntity* entity = new MultiEntity();
	entity->AddEntity(EntityType::HULL);
	entity->AddEntity(EntityType::CANON);
	entity->SetMultiEntityType(MultiEntityType::PLAYER);

	FindPlayerSpawn(entity);

	uint32_t id = NetworkIdManager::GetNewId();
	m_mapEndpointEntities[endpointData].insert(entity);
	m_mapNetworkIdEntities[id] = entity;
	m_mapEntitiesNetworkId[entity] = id;
	m_mapMultiEntityTypeEntities[entity->GetMultiEntityType()].insert(entity);
	m_mapEntitiesEndpoints[entity] = endpointData;

	PayloadEntityInfo payload;
	payload.entityData = entity->GetNetworkEntityData();
	payload.entityData.networkId = id;

	Server* server = App::GetInstance().GetServer();
	server->SendEntityInfo(payload, endpointData);

	payload.entityData.multiEntityType = MultiEntityType::CLIENT_PLAYER;
	server->SendEntityInfo(payload, endpointData, false, true);

	SendGameTimer();
}

void GameManager::DeletePlayerProtocol(PayloadPlayerDeath payload, PacketManagerData* endpointData)
{
	auto it = m_mapNetworkIdEntities.find(payload.networkId);
	if (it == m_mapNetworkIdEntities.end())
		return;
	MultiEntity* entity = it->second;
	RemoveEntity(entity,endpointData);
	NewPlayerProtocol(endpointData);
}

void GameManager::HandleCollision(std::unordered_map<MultiEntity*, std::unordered_set<MultiEntity*>>& collisions)
{
	for (auto& it : collisions)
	{
		MultiEntity* entity = it.first;
		MultiEntityType type = entity->GetMultiEntityType();
		
		bool deleted = false;

		for (MultiEntity* entityCollided : it.second)
		{
			
			MultiEntityType typeCollided;
			if (entityCollided == nullptr)
				typeCollided = MultiEntityType::DEFAULT;
			else 
				typeCollided = entityCollided->GetMultiEntityType();

			switch (type)
			{
			case MultiEntityType::PLAYER:
				switch (typeCollided)
				{
				case MultiEntityType::PLAYER:
				case MultiEntityType::OBSTACLE:
					SendCollide(entity, entityCollided, CollisionType::REPULSE);
					break;
				case MultiEntityType::PROJECTILLE:
					SendCollide(entity, entityCollided,CollisionType::DAMAGE);
					break;
				default:
					break;
				}
				break;
			case MultiEntityType::PROJECTILLE:
				switch (typeCollided)
				{
				case MultiEntityType::PLAYER:
				case MultiEntityType::OBSTACLE:
				case MultiEntityType::DEFAULT:
					RemoveEntity(entity,m_mapEntitiesEndpoints[entity]);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}
}

void GameManager::SendGameTimer()
{
	PayloadGameTimer payload;
	payload.time = m_baseGameTimer - m_currentGameTimer;
	App::GetInstance().GetServer()->SendGameTimer(payload);
}

void GameManager::FindPlayerSpawn(MultiEntity* entity)
{
	DirectX::XMFLOAT3 pos = { 0.f,0.f,0.f };
	pos.x = m_mapPositionDistribution(m_randomGen);
	pos.z = m_mapPositionDistribution(m_randomGen);
	entity->SetPosition(pos);

	std::unordered_set<MultiEntity*> obstacles;
	auto it1 = m_mapMultiEntityTypeEntities.find(MultiEntityType::OBSTACLE);
	if (it1 == m_mapMultiEntityTypeEntities.end())
		obstacles = {};
	else
		obstacles = it1->second;

	std::unordered_set<MultiEntity*> otherPlayers;
	auto it2 = m_mapMultiEntityTypeEntities.find(MultiEntityType::PLAYER);
	if (it2 == m_mapMultiEntityTypeEntities.end())
		otherPlayers = {};
	else
		otherPlayers = it2->second;

	std::vector<EntityWrapper*>& subEntities = entity->GetEntities();

	bool canSpawnHere = false;
	bool collided = false;
	while (canSpawnHere == false)
	{
		for (MultiEntity* obstacle : obstacles)
		{
			if (m_colliderManager.CheckCollisionBetweenWrappers(subEntities, obstacle->GetEntities()))
			{
				collided = true;
				break;
			}
		}

		if (collided)
		{
			pos.x = m_mapPositionDistribution(m_randomGen);
			pos.z = m_mapPositionDistribution(m_randomGen);
			entity->SetPosition(pos);
			continue;
		}

		for (MultiEntity* otherPlayer : otherPlayers)
		{
			EntityWrapper* otherHull = otherPlayer->GetEntity(EntityType::HULL);
			DirectX::XMFLOAT3 otherPos = otherHull->GetPosition();
			float dx = otherPos.x - pos.x;
			float dz = otherPos.z - pos.z;
			if (sqrt(dx*dx + dz*dz) < m_noPlayerSpawnRadius)
			{
				collided = true;
				break;
			}
		}

		if (collided)
		{
			pos.x = m_mapPositionDistribution(m_randomGen);
			pos.z = m_mapPositionDistribution(m_randomGen);
			entity->SetPosition(pos);
			continue;
		}

		canSpawnHere = true;
	}
}

void GameManager::CommunicateNewPositions()
{
	Server* server = App::GetInstance().GetServer();

	for (auto& it : m_mapEndpointEntities)
	{
		for (MultiEntity* entity : it.second)
		{
			PayloadEntityInfo payload;
			payload.entityData = entity->GetNetworkEntityData();
			payload.entityData.networkId = m_mapEntitiesNetworkId[entity];
			payload.entityData.networkIdInit = true;

			server->SendEntityInfo(payload, it.first);
		}
	}
}

void GameManager::AddEntity(PayloadEntityInfo payload, PacketManagerData* endpointData)
{
	uint32_t id = NetworkIdManager::GetNewId();

	MultiEntity* entity = new MultiEntity();
	for (size_t i = 0; i < payload.entityData.wrapperCount; i++)
	{
		EntityWrapperData entityWrapperData = payload.entityData.entityWrapperDatas[i];
		EntityWrapper* entityWrapper = entity->AddEntity(entityWrapperData.entityType);
		entityWrapper->SetDirection(entityWrapperData.lookTo);
		entityWrapper->SetPosition(entityWrapperData.position);
	}

	entity->SetMultiEntityType(payload.entityData.multiEntityType);

	m_mapEndpointEntities[endpointData].insert(entity);
	m_mapNetworkIdEntities[id] = entity;
	m_mapEntitiesNetworkId[entity] = id;
	m_mapMultiEntityTypeEntities[entity->GetMultiEntityType()].insert(entity);
	m_mapEntitiesEndpoints[entity] = endpointData;

	payload.entityData.networkId = id;
	payload.entityData.networkIdInit = true;

	Server* server = App::GetInstance().GetServer();
	server->SendEntityInfo(payload, endpointData);

	PayloadGiveEntityNetworkId payloadGiveId;
	payloadGiveId.localId = payload.entityData.localId;
	payloadGiveId.networkId = id;
	server->SendGiveEntityIdNetwork(payloadGiveId, endpointData);
}

void GameManager::UpdateEntity(PayloadEntityInfo payload, PacketManagerData* endpointData)
{
	MultiEntity* entity = m_mapNetworkIdEntities[payload.entityData.networkId];
	if (entity == nullptr)
		return;
	for (size_t i = 0; i < payload.entityData.wrapperCount; i++)
	{
		EntityWrapperData entityWrapperData = payload.entityData.entityWrapperDatas[i];
		EntityWrapper* entityWrapper = entity->GetEntity(entityWrapperData.entityType);
		entityWrapper->SetDirection(entityWrapperData.lookTo);
		entityWrapper->SetPosition(entityWrapperData.position);
	}

	Server* server = App::GetInstance().GetServer();
	server->SendEntityInfo(payload, endpointData);
}

void GameManager::RemoveEntity(PayloadEntityInfo payload, PacketManagerData* endpointData)
{
	RemoveEntity(m_mapNetworkIdEntities[payload.entityData.networkId], endpointData);
}

void GameManager::SendCollide(MultiEntity* entity, MultiEntity* CollidedWithEntity, CollisionType collisionType)
{
	uint32_t id = m_mapEntitiesNetworkId[entity];
	uint32_t idCollidedWith = m_mapEntitiesNetworkId[CollidedWithEntity];
	PayloadCollide payload = { id, idCollidedWith, collisionType };
	PacketManagerData* endpoint = m_mapEntitiesEndpoints[entity];
	App::GetInstance().GetServer()->SendCollide(payload, endpoint);
}

void GameManager::ClearDeadEntities()
{
	for (MultiEntity* entity : m_deadEntities)
	{
		uint32_t id = m_mapEntitiesNetworkId[entity];
		m_mapEntitiesNetworkId.erase(entity);
		PacketManagerData* endpoint = m_mapEntitiesEndpoints[entity];
		m_mapMultiEntityTypeEntities[entity->GetMultiEntityType()].erase(entity);
		m_mapEntitiesEndpoints.erase(entity);
		m_mapEndpointEntities[endpoint].erase(entity);
		m_mapNetworkIdEntities.erase(id);
		CPU_DELPTR(entity);
	}
	m_deadEntities.clear();
}

void GameManager::Reset()
{
	m_currentGameTimer = 0.f;
	for (auto& it : m_mapNetworkIdEntities)
	{
		RemoveEntity(it.second, nullptr);
	}

	ClearDeadEntities();
	NetworkIdManager::ResetIds();

	m_mapManager.GenerateMap(this);

	for (auto& it : m_mapEndpointEntities)
	{
		PacketManagerData* client = it.first;
		if (client == nullptr) //server
			return;
		NewPlayerProtocol(client);
	}
}