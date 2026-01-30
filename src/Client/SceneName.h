#pragma once
#include "Scene.h"
#include "Client.h"
#include <iostream>

class SceneName : public Scene
{
public:
	SceneName();
	~SceneName();

	void Update() override;
	void OnRender(int pass) override;
private:
	void WritingName(int c);
	void ValidateName(int c, Client* client);
	void StatusMsg();
private:
	std::string statusMsg = "Your name must be unique";
	std::string m_name;
};

