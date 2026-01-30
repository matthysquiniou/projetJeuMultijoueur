#pragma once
#include <unordered_map>
#include "MultiEntity.h"
#include "PacketManagerData.h"

class ColliderManager
{
public:
	ColliderManager();
	~ColliderManager();

	void CheckCollision(std::unordered_map<MultiEntityType, std::unordered_set<MultiEntity*>>& entities, std::unordered_map<MultiEntity*, PacketManagerData*>& mapEntitiesEndPoint);
	bool CheckCollisionBetweenWrappers(std::vector<EntityWrapper*>& subEntities1, std::vector<EntityWrapper*>& subEntities2);
private:
	void CheckCollisionBetweenTypes(std::unordered_set<MultiEntity*>& entities1, std::unordered_set<MultiEntity*>& entities2, std::unordered_map<MultiEntity*, PacketManagerData*>& mapEntitiesEndPoint);
	void CheckProhjectillesUnder0Y(std::unordered_set<MultiEntity*>& projectilles);
	void UpdateNoCollisionsFlag();
private:
	std::unordered_map<MultiEntity*,std::unordered_set<MultiEntity*>> m_collisionsOccurred;
	std::vector<std::pair<MultiEntityType, MultiEntityType>> m_collisionsToCheck;

	std::unordered_map<MultiEntity*, float> m_mapNoCollisionsFlag;
	float m_noCollisionFlagBaseTimer = 0.5f;
};

