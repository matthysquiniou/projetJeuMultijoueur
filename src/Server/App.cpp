#include "App.h"
#include "Server.h"
#include <cpu-engine.h>

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
	delete m_pServer;
}


Server* App::GetServer()
{
	return m_pServer;
}

GameManager& App::GetGameManager()
{
	return m_gameManager;
}

void App::OnStart()
{
	m_pServer = new Server();
	m_pServer->Run();
}

void App::OnUpdate()
{
	m_pServer->Update();
	m_gameManager.Update();
	if (cpuInput.IsKeyDown(VK_ESCAPE))
		cpuEngine.Quit();
}

void App::OnExit()
{
}

void App::OnRender(int pass)
{
}