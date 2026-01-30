#include "SceneConnexion.h"
#include "SceneName.h"

#include "pch.h"
#include "WrapperInput.h"
#include "Client.h"
#include <iostream>
#include <regex>
#include "UIWrapper.h"


SceneConnexion::SceneConnexion()
{
	AddUI(EntityType::TITLE);
}

SceneConnexion::~SceneConnexion()
{

}

void SceneConnexion::Update()
{
	Client* client = App::GetInstance().GetClient();
	SceneConnexion::WritingIp();

	//Enter Choice
	if (cpuInput.IsKeyDown(13))
	{
		SceneConnexion::Connect(client);
	}

	SceneConnexion::ConnexionStatusMsg(client);
}

void SceneConnexion::WritingIp()
{
	//backspace
	if (cpuInput.IsKeyDown(8) && m_queryIndex > 0) {
		
		if ((m_queryIndex) % 4 == 0 && m_queryIndex < 17)
		{
			m_queryIndex--;
		}
		m_queryIndex--;
		m_placeh[m_queryIndex] = (char)95;
	}

	if (m_queryIndex < 21)
	{
		int c = WrapperInput::GetKey();

		if (c != -1 && c != 8)
		{

			if (c >= 48 && c <= 57)
			{
				m_placeh[m_queryIndex] = (char)c;
				m_queryIndex++;

				if ((m_queryIndex + 1) % 4 == 0 && m_queryIndex < 16)
				{
					m_queryIndex++;
				}
			}
		};
	}
}

bool SceneConnexion::FormatIp()
{
	//split around the ":"
	std::regex pattern(":");
	std::sregex_token_iterator it(
		m_placeh.begin(), m_placeh.end(), pattern, -1);
	std::sregex_token_iterator end;
	std::vector<std::string> tokens;

	while (it != end) {
		tokens.push_back(*it++);
	}

	// split around "."
	std::string ipTemp = tokens[0];
	std::regex ip_regex(R"((\d+)\.(\d+)\.(\d+)\.(\d+))");
	std::smatch match;

	if (!std::regex_match(ipTemp, match, ip_regex)) {
		m_connexionStatus = "Invalid IP Format";
		return false;
	}

	m_ip = "";

	for (int i = 1; i <= 4; ++i) {
		int octet = std::stoi(match[i]); // removes leading zeros

		//validate IPv4 range
		if (octet < 0 || octet > 255) {
			m_connexionStatus = "Invalid IP Format";
			return false;
		}

		m_ip += std::to_string(octet);
		if (i < 4)
			m_ip += '.';
	}

	m_ipPcstr = m_ip.c_str();
	m_port = std::stoi(tokens[1]);
	return true;
}

void SceneConnexion::Connect(Client* client)
{
	// blank = 
	if (m_placeh == "___.___.___.___:_____") {
		m_ip = "127.0.0.1";
		m_ipPcstr = m_ip.c_str();
		m_port = 1888;
	}
	else
	{
		if (SceneConnexion::FormatIp() == false)
			return;
	}

	//start connexion here
	client->ConnectTo(m_ipPcstr, m_port);
}

void SceneConnexion::ConnexionStatusMsg(Client* client)
{

	if (client->IsClientRunning()) {
		m_connexionStatus = "Connecting...";
		m_connecting = true;
	}

	if (m_connecting && client->IsClientRunning() == false) {
		m_connexionStatus = "Connection Failed !";
	}

	if (client->IsConnectedToServer()) {
		m_connexionStatus = "Connected !";
		App::GetInstance().SwitchGameState(GameState::NAME);
	}
}

void SceneConnexion::OnRender(int pass)
{
	cpu_font& font = App::GetInstance().GetFont();


	// RENDER PAGE CONNEXION
	std::string performances = CPU_STR(cpuTime.fps) + " fps ";
	std::string ipQuery = "ENTER SERVER IP:PORT";
	std::string title = "WORLD OF SHIP";

	cpuDevice.DrawText(&font, performances.c_str(), cpuDevice.GetWidth() * 0.05f, 10, CPU_TEXT_LEFT);
	
	cpuDevice.DrawText(&font, ipQuery.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.4, CPU_TEXT_CENTER);
	cpuDevice.DrawText(&font, m_placeh.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.5, CPU_TEXT_CENTER);
	cpuDevice.DrawText(&font, m_connexionStatus.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.8, CPU_TEXT_CENTER);
}


