#pragma once
#include <unordered_map>
#include <cpu-engine.h>
#include "MeshFactory.h"

class MeshRegister
{
public:
    static bool HasMesh(EntityType entityType);
    static cpu_mesh& GetMesh(EntityType entityType);

private:
    static cpu_mesh& RegisterMesh(EntityType entityType);

private:
    static std::unordered_map<EntityType, cpu_mesh> m_mapMesh;
};