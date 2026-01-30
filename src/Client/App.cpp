#include "App.h"
#include <cpu-engine.h>
#include "Client.h"
#include "Util.h"

#include "SceneGameplay.h"
#include "SceneConnexion.h"
#include "SceneName.h"

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);
}

App::~App()
{
	delete m_pClient;
}

GameState App::GetGameState()
{
	return m_gameState;
}

void App::SwitchGameState(GameState gameState)
{
	switch (gameState)
	{
	case GameState::CONNECT:
		m_sceneManager.SetSceneActive<SceneConnexion>(true);
		m_sceneManager.SetSceneActive<SceneName>(false);
		m_sceneManager.SetSceneActive<SceneGameplay>(false);
		break;
	case GameState::NAME:
		m_sceneManager.SetSceneActive<SceneConnexion>(false);
		m_sceneManager.SetSceneActive<SceneName>(true);
		m_sceneManager.SetSceneActive<SceneGameplay>(false);
		break;
	case GameState::PLAY:
		Util::SetCursorPositionCenter();
		Util::ResetCursorDelta();
		Util::SetCursorVisible(false);
		m_sceneManager.SetSceneActive<SceneConnexion>(false);
		m_sceneManager.SetSceneActive<SceneName>(false);
		m_sceneManager.SetSceneActive<SceneGameplay>(true);
		break;
	default:
		break;
	}
	m_gameState = gameState;
}

void App::HandleNetworkEntity(NetworkEntityData& entityData)
{
	m_sceneManager.HandleNetworkEntity<SceneGameplay>(entityData);
}

void App::HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload)
{
	m_sceneManager.HandleGiveEntityNetworkId<SceneGameplay>(payload);
}

void App::HandleCollide(PayloadCollide& payload)
{
	m_sceneManager.HandleCollide<SceneGameplay>(payload);
}

void App::HandleGameTimer(PayloadGameTimer& payload)
{
	m_sceneManager.HandleGameTimer<SceneGameplay>(payload);
}

void App::Reset()
{
	SwitchGameState(GameState::CONNECT);
	m_sceneManager.Reset();
}

SceneManager& App::GetSceneManager()
{
	return m_sceneManager;
}

Client* App::GetClient()
{
	return m_pClient;
}

cpu_font& App::GetFont()
{
	return m_font;
}

cpu_font& App::GetFontChat()
{
	return m_fontChat;
}

void App::OnStart()
{
	m_font.Create(20);
	m_fontChat.Create(15);
	m_pClient = new Client();
	SwitchGameState(GameState::CONNECT);
}

void App::OnUpdate()
{
	m_pClient->Update();
	m_sceneManager.Update();
	m_controller.Update(m_sceneManager);

	if (cpuInput.IsKeyDown(VK_ESCAPE))
		cpuEngine.Quit();
}

void App::OnExit()
{ 
	Util::SetCursorVisible(true);
}

void App::OnRender(int pass)
{
	m_sceneManager.OnRender(pass);
}