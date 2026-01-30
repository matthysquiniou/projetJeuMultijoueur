#pragma once
#include <unordered_map>
#include <unordered_set>
#include <cpu-engine.h>
#include "EntityWrapper.h"
#include "EntityTag.h"
#include "NetworkEntityData.h"

class MultiEntity
{
public:
    MultiEntity();
    ~MultiEntity();

    virtual void Update() {};

    void SetVisible(bool visible);
    void SetDirection(DirectX::XMFLOAT3 direction);
    void SetPosition(DirectX::XMFLOAT3 position);
    void Move(float distance);
    void AddYPR(float yaw, float pitch = 0.f, float roll = 0.f);

    uint32_t GetId();

    bool HasEntity(EntityType entityType);
    EntityWrapper* GetEntity(EntityType entityType);
    std::vector<EntityWrapper*>& GetEntities();
    EntityWrapper* AddEntity(EntityType entityType);
    uint8_t GetEntityCount();

    NetworkEntityData GetNetworkEntityData();

    void AddTag(EntityTag tag);
    void AddTags(std::vector<EntityTag> tags);
    void RemoveTag(EntityTag tag);
    void RemoveTags(std::vector<EntityTag> tags);
    bool HasTag(EntityTag tag);
    bool HasTags(std::vector<EntityTag> tags, bool logicAnd);

    void SetMultiEntityType(MultiEntityType multiEntityType);
    MultiEntityType GetMultiEntityType();
	
private:
	std::unordered_map<EntityType,EntityWrapper*> m_mapEntities;
    std::vector<EntityWrapper*> m_entities;
    std::unordered_set<EntityTag> m_tags;

    MultiEntityType m_multiEntityType = MultiEntityType::DEFAULT;

    uint32_t m_id;
    static uint32_t m_lastId;
};