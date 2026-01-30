#pragma once
#include <cpu-engine.h>
#include <EntityType.h>
#include "EntityWrapperData.h"

class EntityWrapper
{
public:
	EntityWrapper(EntityType entityType);
	~EntityWrapper();

	void SetVisible(bool visible);
	void SetDirection(DirectX::XMFLOAT3& direction);
	void Move(float distance);
	void AddYPR(float yaw, float pitch = 0.f, float roll = 0.f);
	void LookAt(DirectX::XMFLOAT3& at);
	void LookTo(DirectX::XMFLOAT3& to);
	void SetPosition(DirectX::XMFLOAT3& position);
	cpu_obb& getObb();
	DirectX::XMFLOAT3& GetPosition();

	EntityWrapperData GetEntityWrapperData();

	cpu_entity* GetCpuEntity();

private:
	void LoadMesh(EntityType entityType);
	void LoadMaterial(EntityType entityType);

protected:
	EntityType m_entityType;
	cpu_entity* m_pEntity;
	cpu_mesh* m_pMesh;
	cpu_material* m_pMaterial;
};

