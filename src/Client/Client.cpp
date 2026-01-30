#include "Client.h"
#include "PacketFactory.h"
#include <iostream>
#include <string>
#include "Scene.h"
#include "SceneManager.h"
#include "SceneGameplay.h"

Client::Client() : PacketManager()
{

}

Client::~Client()
{
}

void Client::ConnectTo(PCSTR ip, uint16_t port)
{
	if (m_connectedToServer)
		return;

	sockaddr_in serverAddr;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;

	if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0)
	{
		return;
	}

	m_serverData.addr = serverAddr;
	PacketData<PayloadConnect> packetData = PacketFactory::MakePacketData(++m_serverData.lastIDSent, m_serverData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, PayloadConnect());

	m_serverData.toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 5, std::bind(&Client::OnCompleteConnected, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		)
	);
	Run();
}

void Client::SendName(std::string name)
{
	PayloadName payload;
	strcpy_s(payload.name, name.c_str());
	PacketData<PayloadName> packetData = PacketFactory::MakePacketData(++m_serverData.lastIDSent, m_serverData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, payload);

	m_serverData.toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 5, std::bind(&Client::OnCompleteName, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		)
	);
}

void Client::AddEntity(PayloadEntityInfo payload)
{
	PacketData<PayloadEntityInfo> packetData = PacketFactory::MakePacketData(++m_serverData.lastIDSent, m_serverData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, payload);

	m_serverData.toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 5, std::bind(&Client::OnCompleteAddEntity, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		)
	);
}

void Client::SendEntityInfo(PayloadEntityInfo payload)
{
	PacketData<PayloadEntityInfo> packetData = PacketFactory::MakePacketData(++m_serverData.lastIDSent, m_serverData.lastFrameIDSent, HeaderFlags::NONE, false, payload);

	m_serverData.toSend.push_back(
		PacketFactory::CreatePacketToSend(packetData, false)
	);
}

void Client::SendPlayerDeath(uint32_t playerId, uint32_t killerId)
{
	PayloadPlayerDeath payload = { playerId, killerId };
	PacketData<PayloadPlayerDeath> packetData = PacketFactory::MakePacketData(++m_serverData.lastIDSent, m_serverData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);

	m_serverData.toSend.push_back(
		PacketFactory::CreatePacketToSend(packetData, true, 5)
	);
}


bool Client::IsClientRunning()
{
	return m_clientRunning;
}

bool Client::IsConnectedToServer()
{
	return m_connectedToServer;
}

void Client::Update()
{
	if (m_clientRunning)
	{
		m_deltaTime = m_clock.GetDeltaTime();
		UpdateFrameId();
		UpdateServerTimeout();
		ProcessIncomingPacket();
		SendPacket(m_serverData);
	}
}

void Client::UpdateServerTimeout()
{
	EnterCriticalSection(&m_incomingCS);
	bool hasReceivedPackets = !m_incomingPackets.empty();
	LeaveCriticalSection(&m_incomingCS);


	if (hasReceivedPackets)
		m_serverTimeoutTimer = 0.f;
	else
	{
		m_serverTimeoutTimer += m_deltaTime;
		if (m_serverTimeoutTimer >= m_serverTimeoutBaseTimer)
		{
			m_serverTimeoutTimer = 0.f;
			App::GetInstance().Reset();
			Stop();
		}
	}
}

void Client::UpdateFrameId()
{
	m_serverData.lastFrameIDSent++;
}

void Client::Run()
{
	StartReceiveThread();
	m_clock.GetDeltaTime();
	m_clientRunning = true;
}

void Client::Stop()
{
	m_clientRunning = false;
	m_connectedToServer = false;
	EndReceiveThread();

	std::queue<Packet> emptyQueue;
	EnterCriticalSection(&m_incomingCS);
	std::swap(m_incomingPackets, emptyQueue);
	LeaveCriticalSection(&m_incomingCS);
}

void Client::HandlePacket(const Packet& packet)
{
	if (packet.data.size() < sizeof(PacketHeader))
		return;

	PacketHeader header;
	std::memcpy(&header, packet.data.data(), sizeof(header));
	PayloadType payloadType = static_cast<PayloadType>(header.payloadType);

	if (m_serverData.addr.sin_addr.S_un.S_addr != packet.from.sin_addr.S_un.S_addr || packet.from.sin_port != m_serverData.addr.sin_port)
		return;

	bool alwaysProcess = static_cast<HeaderFlags>(header.flag & 0x01) == HeaderFlags::ALWAYS_PROCESS;
	if (alwaysProcess && m_serverData.alwaysProccedIds.contains(header.id))
		return;
	else if (alwaysProcess)
		m_serverData.alwaysProccedIds.insert(header.id);

	bool isNewer = static_cast<int32_t>(header.id - m_serverData.lastIDReceived) > 0;
	bool isSameOrNewerFrame = static_cast<int32_t>(header.frameId - m_serverData.lastFrameIDReceived) >= 0;

	if (isNewer)
	{
		if (m_serverData.lastIDReceived > header.id)
			m_serverData.alwaysProccedIds.clear();
		m_serverData.lastIDReceived = header.id;
	}

	if (isSameOrNewerFrame || alwaysProcess)
	{
		if (isSameOrNewerFrame)
			m_serverData.lastFrameIDReceived = header.frameId;

		switch (payloadType)
		{
		case PayloadType::PING: ProcessTypedPacket<PayloadPing, Client>(packet, m_serverData, this, &Client::HandlePacketPing); break;
		case PayloadType::ACK: ProcessTypedPacket<PayloadACK, Client>(packet, m_serverData, this, &Client::HandleReceivePacketACK); break;
		case PayloadType::COMFIRM_NAME: ProcessTypedPacket<PayloadComfirmName, Client>(packet, m_serverData, this, &Client::HandlePacketComfirmName); break;
		case PayloadType::ENTITY_INFO: ProcessTypedPacket<PayloadEntityInfo, Client>(packet, m_serverData, this, &Client::HandlePacketEntityInfo); break;
		case PayloadType::GIVE_ENTITY_NETWORK_ID: ProcessTypedPacket<PayloadGiveEntityNetworkId, Client>(packet, m_serverData, this, &Client::HandlePacketGiveEntityNetworkId); break;
		case PayloadType::COLLIDE: ProcessTypedPacket<PayloadCollide, Client>(packet, m_serverData, this, &Client::HandlePacketCollide); break;
		case PayloadType::CHAT: ProcessTypedPacket<PayloadChat, Client>(packet, m_serverData, this, &Client::HandlePacketChat); break;
		case PayloadType::GAME_TIMER: ProcessTypedPacket<PayloadGameTimer, Client>(packet, m_serverData, this, &Client::HandlePacketGameTimer); break;
		}
	}
}

void Client::HandlePacketPing(PayloadPing& payload, PacketManagerData& endpointData)
{
}

void Client::HandlePacketComfirmName(PayloadComfirmName& payload, PacketManagerData& endpointData)
{
	App::GetInstance().SwitchGameState(GameState::PLAY);
}

void Client::HandlePacketEntityInfo(PayloadEntityInfo& payload, PacketManagerData& endpointData)
{
	App::GetInstance().HandleNetworkEntity(payload.entityData);
}

void Client::HandlePacketGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload, PacketManagerData& enpointData)
{
	App::GetInstance().HandleGiveEntityNetworkId(payload);
}

void Client::HandlePacketCollide(PayloadCollide& payload, PacketManagerData& endpointData)
{
	App::GetInstance().HandleCollide(payload);
}

void Client::HandlePacketChat(PayloadChat& payload, PacketManagerData& endpointData)
{
	Scene* pScene = App::GetInstance().GetSceneManager().GetScene<SceneGameplay>();
	std::string msg(payload.message, strlen(payload.message));
	pScene->UpdateChat(msg);
}

void Client::HandlePacketGameTimer(PayloadGameTimer& payload, PacketManagerData& endpointData)
{
	App::GetInstance().HandleGameTimer(payload);
}

void Client::OnCompleteConnected(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& server)
{
	switch (packetStatus)
	{
	case PacketStatus::Acknowledged:
		m_connectedToServer = true;
		std::cout << "- connected -" << std::endl;
		break;
	case PacketStatus::Expired:
		std::cout << "- connect failed -" << std::endl;
		Stop();
		break;
	default:
		break;
	}
}

void Client::OnCompleteName(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData)
{
	switch (packetStatus)
	{
	case PacketStatus::Acknowledged:
		std::cout << "- name ack -" << std::endl;
		break;

	case PacketStatus::Expired:
		std::cout << "- name expired -" << std::endl;
		break;
	default:
		break;
	}
}

void Client::OnCompleteAddEntity(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData)
{
}

