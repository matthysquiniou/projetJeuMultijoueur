#include "SceneName.h"
#include "App.h"
#include "WrapperInput.h"
#include "Client.h"
#include "Util.h"

SceneName::SceneName()
{
	AddUI(EntityType::TITLE);
}

SceneName::~SceneName()
{

}

void SceneName::Update()
{
	Client* client = App::GetInstance().GetClient();
	int c = WrapperInput::GetKey();

	SceneName::WritingName(c);
	SceneName::ValidateName(c, client);
	SceneName::StatusMsg();
}

void SceneName::WritingName(int c)
{
	if (c == 13) return; // ENTER isnt a character to type

	if (m_name.length() < 15) {
		if (c != -1 && c != 8) // No empty c or Backspace
		{
			if (c < 65 || c > 90) return; // Stuck between A and Z
			m_name += (char)(c);
		}
	}
	
	if (c == 8 && m_name.length() > 0) {
		m_name.pop_back();
	}
}

void SceneName::ValidateName(int c, Client* client)
{
	// Check if  another player has this name
	if (c == 13) {
		if (m_name.length() == 0) {
			statusMsg = "Name can't be empty !";
			return;
		}
		statusMsg = "Checking...";
		client->SendName(m_name);
	}

}

void SceneName::StatusMsg()
{
	if (false /* name not unqiue*/) {
		statusMsg = "Name already used on this server";
	}
}

void SceneName::OnRender(int pass)
{
	cpu_font& font = App::GetInstance().GetFont();

	// RENDER NAME QUERY
	std::string performances = CPU_STR(cpuTime.fps) + " fps ";

	std::string nameQuery = "ENTER YOUR NAME";

	cpuDevice.DrawText(&font, performances.c_str(), cpuDevice.GetWidth() * 0.05f, cpuDevice.GetHeight() * 0.05, CPU_TEXT_LEFT);
	cpuDevice.DrawText(&font, nameQuery.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.4, CPU_TEXT_CENTER);
	cpuDevice.DrawText(&font, m_name.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.5, CPU_TEXT_CENTER);
	cpuDevice.DrawText(&font, statusMsg.c_str(), cpuDevice.GetWidth() * 0.5, cpuDevice.GetHeight() * 0.8, CPU_TEXT_CENTER);
}

