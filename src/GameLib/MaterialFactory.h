#pragma once
#include <cpu-engine.h>
#include "EntityType.h"
#include "Shader.h"

class MaterialFactory
{
public:
	static cpu_material Create(EntityType entityType);
};
