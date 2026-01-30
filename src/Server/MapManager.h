#pragma once
#include <vector>
#include <random>
#include <cpu-engine.h>
#include "EntityType.h"

class GameManager;
class MultiEntity;

class MapManager
{
public:
	MapManager();
	~MapManager();
	void GenerateMap(GameManager* gameManager);
private:
	MultiEntity* GenerateObstacle(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 YPR, EntityType obstacleType);
	DirectX::XMFLOAT3 GetRotation();
	std::vector<MultiEntity*> GetMap1();

private:
	std::vector<DirectX::XMFLOAT3> m_rotations;
	std::uniform_int_distribution<> m_indexRotationsDistribution;
	std::mt19937 m_randomGen;
};

