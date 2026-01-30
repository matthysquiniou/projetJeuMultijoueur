#include "EntityWrapper.h"
#include "MeshRegister.h"
#include "MaterialRegister.h"

EntityWrapper::EntityWrapper(EntityType entityType)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_entityType = entityType;
	LoadMesh(entityType);
	LoadMaterial(entityType);
}

EntityWrapper::~EntityWrapper()
{
	cpuEngine.Release(m_pEntity);
}

void EntityWrapper::SetVisible(bool visible)
{
	m_pEntity->visible = visible;
}

void EntityWrapper::SetDirection(DirectX::XMFLOAT3& direction)
{
	m_pEntity->transform.dir = direction;
}

void EntityWrapper::Move(float distance)
{
	m_pEntity->transform.Move(distance);
}

void EntityWrapper::AddYPR(float yaw, float pitch, float roll)
{
	m_pEntity->transform.AddYPR(yaw,pitch,roll);
}

void EntityWrapper::LookAt(DirectX::XMFLOAT3& at)
{
	m_pEntity->transform.LookAt(at.x,at.y,at.z);
}

void EntityWrapper::LookTo(DirectX::XMFLOAT3& to)
{
	m_pEntity->transform.LookTo(to);
}

void EntityWrapper::SetPosition(DirectX::XMFLOAT3& position)
{
	m_pEntity->transform.SetPosition(position.x, position.y, position.z);
}

cpu_obb& EntityWrapper::getObb()
{
	m_pEntity->UpdateWorld(cpuEngine.GetCamera(),cpuDevice.GetWidth(),cpuDevice.GetHeight());
	return m_pEntity->obb;
}

DirectX::XMFLOAT3& EntityWrapper::GetPosition()
{
	return m_pEntity->transform.pos;
}

EntityWrapperData EntityWrapper::GetEntityWrapperData()
{
	EntityWrapperData entityWrapperData = {m_pEntity->transform.pos,m_pEntity->transform.dir,m_entityType };
	return entityWrapperData;
}

cpu_entity* EntityWrapper::GetCpuEntity()
{
	return m_pEntity;
}

void EntityWrapper::LoadMesh(EntityType entityType)
{
	m_pMesh = &MeshRegister::GetMesh(entityType);
	m_pEntity->pMesh = m_pMesh;
}

void EntityWrapper::LoadMaterial(EntityType entityType)
{
	m_pMaterial = &MaterialRegister::GetMaterial(entityType);
	m_pEntity->pMaterial = m_pMaterial;
}


