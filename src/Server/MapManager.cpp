#include "MapManager.h"
#include "GameManager.h"

MapManager::MapManager()
{

	std::vector<float> radiantDiv2 = {
		0.f,
		DirectX::XM_PIDIV4,
		DirectX::XM_PIDIV2,
		DirectX::XM_PIDIV4 * 3.f,
		DirectX::XM_PI,
		DirectX::XM_PIDIV4 * 5.f,
		DirectX::XM_PIDIV2 * 3.f,
		DirectX::XM_PIDIV4 * 7.f,
	};

	for (float yaw : radiantDiv2)
	{
		m_rotations.push_back({ yaw,0.f,0.f });
	}

	m_randomGen = std::mt19937(std::random_device()());
	m_indexRotationsDistribution = std::uniform_int_distribution<>(0, m_rotations.size() - 1);
}

MapManager::~MapManager()
{
}

void MapManager::GenerateMap(GameManager* gameManager)
{
	std::vector<MultiEntity*> map = GetMap1();

	for (size_t i = 0; i < map.size(); i++)
	{
		MultiEntity* entity = map[i];
		gameManager->AddEntity(entity);
	}
}

MultiEntity* MapManager::GenerateObstacle(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 YPR, EntityType obstacleType)
{
	MultiEntity* entity = new MultiEntity();
	entity->SetMultiEntityType(MultiEntityType::OBSTACLE);
	entity->AddEntity(obstacleType);
	entity->AddYPR(YPR.x,YPR.y,YPR.z);
	entity->SetPosition(position);
	return entity;
}

DirectX::XMFLOAT3 MapManager::GetRotation()
{
	int rotationIndex = m_indexRotationsDistribution(m_randomGen);
	return m_rotations[rotationIndex];
}

std::vector<MultiEntity*> MapManager::GetMap1() // hard coded map maybe could do generation
{
	std::vector<MultiEntity*> map = {};


	map.push_back(GenerateObstacle({ 100.f,0.f,0.f }, { 0.f,0.f,0.f }, EntityType::OBSTACLE_WALL));
	map.push_back(GenerateObstacle({ -100.f,0.f,0.f }, { 0.f,0.f,0.f }, EntityType::OBSTACLE_WALL));
	map.push_back(GenerateObstacle({ 0.f,0.f,100.f }, { DirectX::XM_PIDIV2,0.f,0.f }, EntityType::OBSTACLE_WALL));
	map.push_back(GenerateObstacle({ 0.f,0.f,-100.f }, { DirectX::XM_PIDIV2,0.f,0.f }, EntityType::OBSTACLE_WALL));


	// ugly maybe will change it latter

	map.push_back(GenerateObstacle({ -80.f, 0.f, -80.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ -50.f, 0.f, -70.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ -20.f, 0.f, -60.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ 10.f, 0.f, -50.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ 40.f, 0.f, -40.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ 70.f, 0.f, -30.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ -80.f, 0.f, 20.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	map.push_back(GenerateObstacle({ 50.f, 0.f, 50.f }, GetRotation(), EntityType::OBSTACLE_BIG));
	
	map.push_back(GenerateObstacle({ -70.f, 0.f, 10.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ -40.f, 0.f, 20.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ -10.f, 0.f, 30.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ 20.f, 0.f, 40.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ 50.f, 0.f, 10.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ -60.f, 0.f, -10.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ 0.f, 0.f, -20.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	map.push_back(GenerateObstacle({ -65.f, 0.f, 5.f }, GetRotation(), EntityType::OBSTACLE_MEDIUM));
	
	
	map.push_back(GenerateObstacle({ -90.f, 0.f, 90.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ -70.f, 0.f, 80.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ -50.f, 0.f, 70.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ -30.f, 0.f, 60.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ -10.f, 0.f, 50.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 10.f, 0.f, 40.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 30.f, 0.f, 30.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 50.f, 0.f, 20.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 70.f, 0.f, 10.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 90.f, 0.f, 0.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 80.f, 0.f, -20.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 60.f, 0.f, -40.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 40.f, 0.f, -60.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 20.f, 0.f, -80.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ 0.f, 0.f, -90.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	map.push_back(GenerateObstacle({ -20.f, 0.f, -75.f }, GetRotation(), EntityType::OBSTACLE_SMALL));
	
	return map; 
}
