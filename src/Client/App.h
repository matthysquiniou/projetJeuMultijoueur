#pragma once
#include <cpu-render.h>
#include "SceneManager.h"
#include "Controller.h"
#include "GameState.h"
#include "UIWrapper.h"

class Client;

class App
{
public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; };

	GameState GetGameState();
	void SwitchGameState(GameState gameState);

	void HandleNetworkEntity(NetworkEntityData& entityData);
	void HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload);
	void HandleCollide(PayloadCollide& payload);
	void HandleGameTimer(PayloadGameTimer& payload);

	void Reset();

	SceneManager& GetSceneManager();
	Client* GetClient();
	cpu_font& GetFont();
	cpu_font& GetFontChat();

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);
private:
	inline static App* s_pApp = nullptr;
	SceneManager m_sceneManager;
	Controller m_controller;
	GameState m_gameState;
	Client* m_pClient;
	UIWrapper* ui;

	cpu_font m_font;
	cpu_font m_fontChat;
};