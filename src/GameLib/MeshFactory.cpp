#include "MeshFactory.h"

cpu_mesh MeshFactory::Create(EntityType entityType)
{
	cpu_mesh mesh;

	switch (entityType)
	{
	case EntityType::HULL:
		mesh.CreateBoatHull(5.f, 1.f, 0.7f, { 0.6f, 0.6f, 0.6f });
		break;
	case EntityType::CANON:
		mesh.CreateCannon(1.5f, 0.3f, 1.f, { 0.3f, 0.3f, 0.3f });
		break;
	case EntityType::PROJECTILLE:
		mesh.CreateSphere();
		break;
	case EntityType::OBSTACLE_WALL:
		mesh.CreateIcebergBlock(200.f,1.f,30.f, {0.7f,0.7f,0.9f});
		break;
	case EntityType::OBSTACLE_BIG:
		mesh.CreateIcebergBlock(15.f, 15.f, 15.f, { 0.7f,0.7f,0.9f });
		break;
	case EntityType::OBSTACLE_MEDIUM:
		mesh.CreateIcebergBlock(7.f, 7.f, 7.f, { 0.7f,0.7f,0.9f });
		break;
	case EntityType::OBSTACLE_SMALL:
		mesh.CreateIcebergBlock(2.f, 2.f, 2.f, { 0.7f,0.7f,0.9f });
		break;
	}

	return mesh;
}