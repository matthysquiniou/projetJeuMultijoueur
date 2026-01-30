#pragma once
#include "GameManager.h"

class Server;

class App
{
public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }
	Server* GetServer();
	GameManager& GetGameManager();

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

private:
	inline static App* s_pApp = nullptr;
	GameManager m_gameManager;
	Server* m_pServer;
	bool m_running = false;
};

