#include "SceneGameplay.h"
#include "Player.h"
#include "Controller.h"
#include "App.h"
#include "Camera.h"
#include "Client.h"
#include "Projectille.h"

SceneGameplay::SceneGameplay()
{	
	//!\ ORDER FOR Z-INDEX /!\\

	AddUI(EntityType::ENGINE_POWER);
	AddUI(EntityType::ENGINE_DIRECTION);
	AddUI(EntityType::BULLET_EMPTY);
	AddUI(EntityType::BULLET);
	AddUI(EntityType::CROSSHAIR);

	AddUI(EntityType::HEALTH0); 
	AddUI(EntityType::HEALTH1); 
	AddUI(EntityType::HEALTH2); 
	AddUI(EntityType::HEALTH3); 
	AddUI(EntityType::HEALTH4); 
	AddUI(EntityType::HEALTH5);
	AddUI(EntityType::HEALTH6);

	AddUI(EntityType::ARROW);
	AddUI(EntityType::ARROW_DOWN);
}

SceneGameplay::~SceneGameplay()
{
	
}

void SceneGameplay::UpdateChat(std::string msg)
{
	m_chatMsg.push_back(msg);
}

void SceneGameplay::OnRender(int pass)
{
	cpu_font& font = App::GetInstance().GetFont();

	int minutes = (int)(m_gameTimer / 60.0f);
	int seconds = (int)(m_gameTimer) % 60;

	std::string gameTimer = CPU_STR(minutes) + " : " + CPU_STR(seconds);
	cpuDevice.DrawText(&font, gameTimer.c_str(), cpuDevice.GetWidth() * 0.05f, 10, CPU_TEXT_LEFT);

	cpu_font& fontChat = App::GetInstance().GetFontChat();

	if (m_chatMsg.size() > 5) {
		m_chatMsg.erase(m_chatMsg.begin());
	}

	if (m_chatMsg.size() == 0) return;
	for (size_t i = 0; i < m_chatMsg.size(); i++)
	{
		cpuDevice.DrawText(&fontChat, m_chatMsg[i].c_str(), cpuDevice.GetWidth() * 0.01, ((cpuDevice.GetHeight() * 0.95) - fontChat.height * (m_chatMsg.size() - 1)) + fontChat.height * i, CPU_TEXT_LEFT);
	}
}

void SceneGameplay::Update()
{
	Scene::Update();
	m_gameTimer -= cpuTime.delta;
}

void SceneGameplay::HandleNetworkEntity(NetworkEntityData& entityData)
{
	auto it = m_mapNetworkIdToEntities.find(entityData.networkId);
	bool exist = it != m_mapNetworkIdToEntities.end();

	if (!exist && !entityData.deleteFlag)
	{
		CreateNetworkEntity(entityData);
		return;
	}

	if (!exist)
		return;

	if (entityData.deleteFlag)
	{
		DeleteNetworkEntity(entityData, it->second);
		return;
	}

	UpdateNetworkEntity(entityData, it->second);
}

void SceneGameplay::HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload)
{
	auto it = m_mapLocalIdToEnties.find(payload.localId);
	if (it == m_mapLocalIdToEnties.end())
		return; // not supposed to happen

	MultiEntity* entity = it->second;
	m_mapNetworkIdToEntities[payload.networkId] = entity;
	m_mapEntitiesToNetworkId[entity] = payload.networkId;
}

void SceneGameplay::HandleCollide(PayloadCollide& payload)
{
	CollisionType collideType = payload.collisionType;
	MultiEntity* entity = m_mapNetworkIdToEntities[payload.networkId];
	MultiEntityType multiEntityType = entity->GetMultiEntityType();

	switch (multiEntityType)
	{
	case MultiEntityType::PLAYER:
	{
		Player* player = dynamic_cast<Player*>(entity);
		if (player == nullptr)
			return;
		switch (collideType)
		{
		case CollisionType::REPULSE:
			player->Repulse();
			if (!player->IsAlive())
				App::GetInstance().GetClient()->SendPlayerDeath(m_mapEntitiesToNetworkId[entity], payload.collidedWithNetworkId);
			break;
		case CollisionType::DAMAGE:
			player->TakeDamage();
			if (!player->IsAlive())
				App::GetInstance().GetClient()->SendPlayerDeath(m_mapEntitiesToNetworkId[entity], payload.collidedWithNetworkId);
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

void SceneGameplay::HandleGameTimer(PayloadGameTimer& payload)
{
	m_gameTimer = payload.time;
}

void SceneGameplay::CreateNetworkEntity(NetworkEntityData& entityData)
{
	MultiEntity* entity;
	switch (entityData.multiEntityType)
	{
	case MultiEntityType::DEFAULT:
		entity = new MultiEntity();
		break;
	case MultiEntityType::PLAYER:
		entity = new Player();
		break;
	case MultiEntityType::CLIENT_PLAYER:
		entity = new Player();
		entity->AddTags({ EntityTag::ACTIVE_PLAYER, EntityTag::CLIENT_OWNED_ENTITY });
		Camera::SwitchCameraMode(CameraMode::FOLLOW_PLAYER);
		break;
	case MultiEntityType::PROJECTILLE:
		entity = new Projectille();
		break;
	case MultiEntityType::OBSTACLE:
		entity = new MultiEntity();
		entity->AddEntity(entityData.entityWrapperDatas[0].entityType);
		break;
	default:
		entity = new MultiEntity();
		break;
	}

	if (entityData.multiEntityType == MultiEntityType::CLIENT_PLAYER)
		entity->SetMultiEntityType(MultiEntityType::PLAYER);
	else
		entity->SetMultiEntityType(entityData.multiEntityType);

	if (m_active)
		entity->SetVisible(true);

	UpdateNetworkEntity(entityData, entity);

	m_mapLocalIdToEnties[entity->GetId()] = entity;
	m_mapNetworkIdToEntities[entityData.networkId] = entity;
	m_mapEntitiesToNetworkId[entity] = entityData.networkId;
}

void SceneGameplay::UpdateNetworkEntity(NetworkEntityData& entityData, MultiEntity* entity)
{
	for (size_t i = 0; i < entityData.wrapperCount; i++)
	{
		EntityWrapperData& entityWrapperData = entityData.entityWrapperDatas[i];
		EntityWrapper* entityWrapper = entity->GetEntity(entityWrapperData.entityType);
		entityWrapper->LookTo(entityWrapperData.lookTo);
		entityWrapper->SetPosition(entityWrapperData.position);
	}
}

void SceneGameplay::DeleteNetworkEntity(NetworkEntityData& entityData, MultiEntity* entity)
{
	m_mapLocalIdToEnties.erase(entity->GetId());
	m_mapNetworkIdToEntities.erase(entityData.networkId);
	m_mapEntitiesToNetworkId.erase(entity);
	CPU_DELPTR(entity);
}
