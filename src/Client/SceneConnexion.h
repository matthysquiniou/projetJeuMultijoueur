#pragma once
#include "Scene.h"
#include <iostream>
#include "Client.h"


class SceneConnexion : public Scene
{
public:
	SceneConnexion();
	~SceneConnexion();

	void Update() override;
	void OnRender(int pass) override;

private:

	void WritingIp();
	bool FormatIp();
	void Connect(Client* client);
	void ConnexionStatusMsg(Client* client);


private:
	std::string m_ip;
	PCSTR m_ipPcstr;
	int m_port;

	int m_queryIndex = 0;
	std::string m_placeh = "___.___.___.___:_____";
	std::string m_connexionStatus = "leave blank for localhost";
	bool m_connecting = false;
};

