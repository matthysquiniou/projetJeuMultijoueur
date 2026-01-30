#include "MaterialRegister.h"

std::unordered_map<EntityType, cpu_material> MaterialRegister::m_mapMaterial;

bool MaterialRegister::HasMaterial(EntityType entityType)
{
    return m_mapMaterial.contains(entityType);
}

cpu_material& MaterialRegister::GetMaterial(EntityType entityType)
{
    auto it = m_mapMaterial.find(entityType);
    if (it != m_mapMaterial.end())
        return it->second;
    else
        return RegisterMaterial(entityType);
}

cpu_material& MaterialRegister::RegisterMaterial(EntityType entityType)
{
    auto [it, _] = m_mapMaterial.emplace(entityType, MaterialFactory::Create(entityType));
    return it->second;
}