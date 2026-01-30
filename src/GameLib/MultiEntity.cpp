#include "MultiEntity.h"

uint32_t MultiEntity::m_lastId = 0;

MultiEntity::MultiEntity()
{
	m_id = ++m_lastId;
}

MultiEntity::~MultiEntity()
{
	m_mapEntities.clear();

	while (!m_entities.empty())
	{
		EntityWrapper* entity = m_entities.back();
		m_entities.pop_back();
		CPU_DELPTR(entity);
	}

	m_entities.clear();
}

void MultiEntity::SetVisible(bool visible)
{
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second; 
		entity->SetVisible(visible);
	}
}

void MultiEntity::SetDirection(DirectX::XMFLOAT3 direction)
{
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second;
		entity->SetDirection(direction);
	}
}

void MultiEntity::SetPosition(DirectX::XMFLOAT3 position)
{
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second;
		entity->SetPosition(position);
	}
}

void MultiEntity::Move(float distance)
{
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second;
		entity->Move(distance);
	}
}

void MultiEntity::AddYPR(float yaw, float pitch, float roll)
{
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second;
		entity->AddYPR(yaw, pitch, roll);
	}
}

uint32_t MultiEntity::GetId()
{
	return m_id;
}

bool MultiEntity::HasEntity(EntityType entityType)
{
	return m_mapEntities.contains(entityType);
}

EntityWrapper* MultiEntity::GetEntity(EntityType entityType)
{
	auto it = m_mapEntities.find(entityType);
	if (it == m_mapEntities.end())
		return nullptr;
	return it->second;
}

std::vector<EntityWrapper*>& MultiEntity::GetEntities()
{
	return m_entities;
}

EntityWrapper* MultiEntity::AddEntity(EntityType entityType)
{
	auto it = m_mapEntities.find(entityType);
	if (it == m_mapEntities.end())
	{
		EntityWrapper* entity = new EntityWrapper(entityType);
		m_mapEntities[entityType] = (entity);
		entity->SetVisible(false);
		m_entities.push_back(entity);
		return entity;
	}
	return it->second;
}

uint8_t MultiEntity::GetEntityCount()
{
	return m_mapEntities.size();
}

NetworkEntityData MultiEntity::GetNetworkEntityData()
{
	NetworkEntityData data;
	uint8_t index = 0;
	for (auto& it : m_mapEntities)
	{
		EntityWrapper* entity = it.second;
		data.entityWrapperDatas[index] = entity->GetEntityWrapperData();
		index++;
	}
	data.wrapperCount = index;
	data.localId = m_id;
	data.multiEntityType = m_multiEntityType;
	return data;
}

void MultiEntity::AddTag(EntityTag tag)
{
	m_tags.insert(tag);
}

void MultiEntity::AddTags(std::vector<EntityTag> tags)
{
	for (size_t i = 0; i < tags.size(); i++)
	{
		m_tags.insert(tags[i]);
	}
}

void MultiEntity::RemoveTag(EntityTag tag)
{
	m_tags.erase(tag);
}

void MultiEntity::RemoveTags(std::vector<EntityTag> tags)
{
	for (size_t i = 0; i < tags.size(); i++)
	{
		m_tags.erase(tags[i]);
	}
}

bool MultiEntity::HasTag(EntityTag tag)
{
	return m_tags.contains(tag);
}

bool MultiEntity::HasTags(std::vector<EntityTag> tags, bool logicAnd)
{
	if (logicAnd)
	{
		for (size_t i = 0; i < tags.size(); i++)
		{
			EntityTag tag = tags[i];
			if (!m_tags.contains(tag))
				return false;
		}
		return true;
	}
	else
	{
		for (size_t i = 0; i < tags.size(); i++)
		{
			EntityTag tag = tags[i];
			if (m_tags.contains(tag))
				return true;
		}
		return false;
	}
}

void MultiEntity::SetMultiEntityType(MultiEntityType multiEntityType)
{
	m_multiEntityType = multiEntityType;
}

MultiEntityType MultiEntity::GetMultiEntityType()
{
	return m_multiEntityType;
}