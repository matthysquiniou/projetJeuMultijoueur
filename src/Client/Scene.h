#pragma once
#include "MultiEntity.h"
#include "NetworkEntityData.h"
#include "Payloads.h"

class Player;
class UIWrapper;

class Scene
{
public:
	Scene();
	~Scene();

	bool IsActive();
	void SetActive(bool active);

	virtual void Update();
	virtual void OnRender(int pass);

	void Reset();

	std::vector<MultiEntity*> GetMultiEntitiesOfTag(EntityTag tag);
	std::vector<MultiEntity*> GetMultiEntitiesOfTags(std::vector<EntityTag> tags, bool logicAnd);
	MultiEntity* GetFirstMultiEntityOfTag(EntityTag tag);
	MultiEntity* GetFirstMultiEntityOfTags(std::vector<EntityTag> tags, bool logicAnd);
	std::unordered_map<EntityType, UIWrapper*> GetMapUI();

	void AddEntity(MultiEntity* entity);

	UIWrapper* AddUI(EntityType entityType);
	UIWrapper* GetUI(EntityType entityType);
	virtual void HandleNetworkEntity(NetworkEntityData& entityData);
	virtual void HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload);
	virtual void HandleCollide(PayloadCollide& payload);
	virtual void HandleGameTimer(PayloadGameTimer& payload);
	virtual void UpdateChat(std::string msg);

private:
	void SendEntitiesInfo();

protected:
	bool m_active = false;
	std::unordered_map<uint32_t, MultiEntity*> m_mapLocalIdToEnties;
	std::unordered_map<uint32_t, MultiEntity*> m_mapNetworkIdToEntities;
	std::unordered_map<MultiEntity*, uint32_t> m_mapEntitiesToNetworkId;
	std::unordered_map<EntityType, UIWrapper*> m_mapUI;
};

