#include "MeshRegister.h"

std::unordered_map<EntityType, cpu_mesh> MeshRegister::m_mapMesh;

bool MeshRegister::HasMesh(EntityType entityType)
{
    return m_mapMesh.contains(entityType);
}

cpu_mesh& MeshRegister::GetMesh(EntityType entityType)
{
    auto it = m_mapMesh.find(entityType);
    if (it != m_mapMesh.end())
        return it->second;
    else
        return RegisterMesh(entityType);
}

cpu_mesh& MeshRegister::RegisterMesh(EntityType entityType)
{
    auto [it, _] = m_mapMesh.emplace(entityType, MeshFactory::Create(entityType));
    return it->second;
}