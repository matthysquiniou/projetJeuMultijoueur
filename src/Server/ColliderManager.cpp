#include "ColliderManager.h"
#include "App.h"

ColliderManager::ColliderManager()
{
	m_collisionsToCheck =
	{
		{MultiEntityType::PLAYER, MultiEntityType::PLAYER},
		{MultiEntityType::PLAYER, MultiEntityType::OBSTACLE},
		{MultiEntityType::PLAYER, MultiEntityType::PROJECTILLE},
		{MultiEntityType::PROJECTILLE, MultiEntityType::OBSTACLE}
	};
}

ColliderManager::~ColliderManager()
{
}

void ColliderManager::CheckCollision(std::unordered_map<MultiEntityType, std::unordered_set<MultiEntity*>>& entities, std::unordered_map<MultiEntity*, PacketManagerData*>& mapEntitiesEndPoint)
{
	for (size_t i = 0; i < m_collisionsToCheck.size(); i++)
	{
		std::pair<MultiEntityType, MultiEntityType> collisionTypeToCheck = m_collisionsToCheck[i];
		auto it1 = entities.find(collisionTypeToCheck.first);
		auto it2 = entities.find(collisionTypeToCheck.second);
		if (it1 == entities.end() || it2 == entities.end())
			continue;
		CheckCollisionBetweenTypes(it1->second, it2->second, mapEntitiesEndPoint);
	}

	auto it = entities.find(MultiEntityType::PROJECTILLE);
	if (it != entities.end())
		CheckProhjectillesUnder0Y(it->second);

	App::GetInstance().GetGameManager().HandleCollision(m_collisionsOccurred);
	m_collisionsOccurred.clear();

	UpdateNoCollisionsFlag();
}

bool ColliderManager::CheckCollisionBetweenWrappers(std::vector<EntityWrapper*>& subEntities1, std::vector<EntityWrapper*>& subEntities2)
{
	for (EntityWrapper* subEntity1 : subEntities1)
	{
		cpu_obb& obb1 = subEntity1->getObb();
		for (EntityWrapper* subEntity2 : subEntities2)
		{
			cpu_obb& obb2 = subEntity2->getObb();
			if (cpu::ObbObb(obb1, obb2))
				return true;
		}
	}
	return false;
}

void ColliderManager::CheckCollisionBetweenTypes(std::unordered_set<MultiEntity*>& entities1, std::unordered_set<MultiEntity*>& entities2, std::unordered_map<MultiEntity*, PacketManagerData*>& mapEntitiesEndPoint)
{
	for (MultiEntity* entity1 : entities1)
	{
		if (m_mapNoCollisionsFlag.contains(entity1))
			continue;

		for (MultiEntity* entity2 : entities2)
		{
			if (m_mapNoCollisionsFlag.contains(entity2))
				continue;

			if (entity1 == entity2)
				continue;

			if (mapEntitiesEndPoint[entity1] == mapEntitiesEndPoint[entity2])
				continue;

			auto it = m_collisionsOccurred.find(entity1);
			if (it != m_collisionsOccurred.end() && it->second.contains(entity2))
				continue;

			std::vector<EntityWrapper*>& subEntities1 = entity1->GetEntities();
			std::vector<EntityWrapper*>& subEntities2 = entity2->GetEntities();
			if (CheckCollisionBetweenWrappers(subEntities1, subEntities2))
			{
				m_collisionsOccurred[entity1].insert(entity2);
				m_collisionsOccurred[entity2].insert(entity1);
				m_mapNoCollisionsFlag[entity1] = 0.f;
				m_mapNoCollisionsFlag[entity2] = 0.f;
			}
		}
	}
}

void ColliderManager::CheckProhjectillesUnder0Y(std::unordered_set<MultiEntity*>& projectilles)
{
	for (MultiEntity* entity : projectilles)
	{

		if (m_collisionsOccurred.contains(entity))
			continue;

		if (entity->GetEntity(EntityType::PROJECTILLE)->GetPosition().y < 0.f)
			m_collisionsOccurred[entity].insert(nullptr);
	}
}

void ColliderManager::UpdateNoCollisionsFlag()
{
	const float& dt = cpuTime.delta;
	std::vector<MultiEntity*> toErase;
	for (auto& it : m_mapNoCollisionsFlag)
	{
		it.second += dt;
		if (it.second >= m_noCollisionFlagBaseTimer)
			toErase.push_back(it.first);
	}

	for (MultiEntity* entity : toErase)
	{
		m_mapNoCollisionsFlag.erase(entity);
	}
}
