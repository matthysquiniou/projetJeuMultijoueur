#pragma once
#include <cpu-engine.h>
#include "EntityType.h"

class MeshFactory
{
public:
	static cpu_mesh Create(EntityType entityType);
};