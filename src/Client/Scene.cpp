#include "Scene.h"
#include "Player.h"
#include "UIWrapper.h"
#include "App.h"
#include "Client.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	for (auto& it : m_mapLocalIdToEnties)
	{
		CPU_DELPTR(it.second);
	}

	for (auto& it:m_mapUI)
	{
		CPU_DELPTR(it.second);
	}

	m_mapEntitiesToNetworkId.clear();
	m_mapLocalIdToEnties.clear();
	m_mapNetworkIdToEntities.clear();
	m_mapUI.clear();
}

bool Scene::IsActive()
{
	return m_active;
}

void Scene::SetActive(bool active)
{
	m_active = active;
	for (auto& it : m_mapLocalIdToEnties)
	{
		it.second->SetVisible(active);
	}

	for (auto& it : m_mapUI) {
		it.second->SetActive(active);
	}
}

void Scene::Update()
{
	for (auto& it : m_mapLocalIdToEnties)
	{
		it.second->Update();
	}

	SendEntitiesInfo();
}

void Scene::OnRender(int pass)
{
	
}

void Scene::Reset()
{
	for (auto& it : m_mapLocalIdToEnties)
	{
		CPU_DELPTR(it.second);
	}

	m_mapEntitiesToNetworkId.clear();
	m_mapLocalIdToEnties.clear();
	m_mapNetworkIdToEntities.clear();
}

std::vector<MultiEntity*> Scene::GetMultiEntitiesOfTag(EntityTag tag)
{
	std::vector<MultiEntity*> entities;
	for (auto& it : m_mapLocalIdToEnties)
	{
		MultiEntity* entity = it.second;
		if (entity->HasTag(tag))
			entities.push_back(entity);
	}
	return entities;
}

std::vector<MultiEntity*> Scene::GetMultiEntitiesOfTags(std::vector<EntityTag> tags, bool logicAnd)
{
	std::vector<MultiEntity*> entities;
	for (auto& it : m_mapLocalIdToEnties)
	{
		MultiEntity* entity = it.second;
		if (entity->HasTags(tags, logicAnd))
			entities.push_back(entity);
	}
	return entities;
}

MultiEntity* Scene::GetFirstMultiEntityOfTag(EntityTag tag)
{
	for (auto& it : m_mapLocalIdToEnties)
	{
		MultiEntity* entity = it.second;
		if (entity->HasTag(tag))
			return entity;
	}
	return nullptr;
}

MultiEntity* Scene::GetFirstMultiEntityOfTags(std::vector<EntityTag> tags, bool logicAnd)
{
	for (auto& it : m_mapLocalIdToEnties)
	{
		MultiEntity* entity = it.second;
		if (entity->HasTags(tags, logicAnd))
			return entity;
	}
	return nullptr;
}

std::unordered_map<EntityType, UIWrapper*> Scene::GetMapUI()
{
	return m_mapUI;
}

void Scene::AddEntity(MultiEntity* entity)
{
	m_mapLocalIdToEnties[entity->GetId()] = entity;

	if (m_active)
		entity->SetVisible(true);

	PayloadEntityInfo payload;
	payload.entityData = entity->GetNetworkEntityData();

	Client* client = App::GetInstance().GetClient();
	client->AddEntity(payload);
}


void Scene::HandleNetworkEntity(NetworkEntityData& entityData)
{
}

void Scene::HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload)
{
}

void Scene::HandleCollide(PayloadCollide& payload)
{
}

void Scene::HandleGameTimer(PayloadGameTimer& payload)
{
}

void Scene::UpdateChat(std::string msg)
{
}

UIWrapper* Scene::AddUI(EntityType entityType)
{
	UIWrapper* ui = new UIWrapper(entityType);
	m_mapUI[entityType] = ui;
	return ui;
}

UIWrapper* Scene::GetUI(EntityType entityType)
{
	auto it = m_mapUI.find(entityType);
	if (it != m_mapUI.end())
		return it->second;

	return nullptr;
}

void Scene::SendEntitiesInfo()
{
	std::vector<MultiEntity*> entities = GetMultiEntitiesOfTag(EntityTag::CLIENT_OWNED_ENTITY);
	Client* client = App::GetInstance().GetClient();

	for (size_t i = 0; i < entities.size(); i++)
	{
		MultiEntity* entity = entities[i];

		auto it = m_mapEntitiesToNetworkId.find(entity);
		if (it == m_mapEntitiesToNetworkId.end())
			continue;

		PayloadEntityInfo payload;
		payload.entityData = entity->GetNetworkEntityData();
		payload.entityData.networkId = it->second;
		payload.entityData.networkIdInit = true;

		client->SendEntityInfo(payload);
	}
}