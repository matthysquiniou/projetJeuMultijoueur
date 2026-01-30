#pragma once
#include "Scene.h"
#include "Controller.h"

class SceneGameplay : public Scene
{
public:
	SceneGameplay();
	~SceneGameplay();
	void OnRender(int pass) override;
	void Update() override;
	void UpdateChat(std::string msg) override;
private:

	void HandleNetworkEntity(NetworkEntityData& entityData) override;
	void HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload) override;
	void HandleCollide(PayloadCollide& payload) override;
	void HandleGameTimer(PayloadGameTimer& payload) override;

private:
	void CreateNetworkEntity(NetworkEntityData& entityData);
	void UpdateNetworkEntity(NetworkEntityData& entityData, MultiEntity* entity);
	void DeleteNetworkEntity(NetworkEntityData& entityData, MultiEntity* entity);

private:
	float m_gameTimer = 0.f;
	int m_maxMsg = 5;
	std::vector<std::string> m_chatMsg;
};

