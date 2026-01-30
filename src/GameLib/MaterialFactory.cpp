#include "MaterialFactory.h"

cpu_material MaterialFactory::Create(EntityType entityType)
{
	cpu_material material;

	switch (entityType)
	{
	case EntityType::HULL:
	case EntityType::CANON:
	case EntityType::PROJECTILLE:
	case EntityType::OBSTACLE_WALL:
	case EntityType::OBSTACLE_BIG:
	case EntityType::OBSTACLE_MEDIUM:
	case EntityType::OBSTACLE_SMALL:

		material.ps = &Shader::SimpleShader;
		break;
	}
	return material;
}