#pragma once
#pragma once
#include <unordered_map>
#include <cpu-engine.h>
#include "MaterialFactory.h"

class MaterialRegister
{
public:
    static bool HasMaterial(EntityType entityType);
    static cpu_material& GetMaterial(EntityType entityType);

private:
    static cpu_material& RegisterMaterial(EntityType entityType);

private:
    static std::unordered_map<EntityType, cpu_material> m_mapMaterial;
};