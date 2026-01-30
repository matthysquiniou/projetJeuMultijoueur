#pragma once
#include <cpu-engine.h>
#include "EntityType.h"
#include "Shader.h"

class TextureFactory
{
public:
	static cpu_texture* Create(EntityType entityType);
};

