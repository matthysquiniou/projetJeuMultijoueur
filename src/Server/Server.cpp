#include "Server.h"
#include "PacketFactory.h"
#include "App.h"
#include <iostream>

Server::Server() : PacketManager()
{
	m_socket.Bind();
	
}

Server::~Server()
{
}

void Server::SendEntityInfo(PayloadEntityInfo payload, PacketManagerData* endpointOwner, bool alsoOwner, bool onlyOwner)
{
	if (onlyOwner)
	{
		PacketData<PayloadEntityInfo> packetData = PacketFactory::MakePacketData(++endpointOwner->lastIDSent, endpointOwner->lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);
		endpointOwner->toSend.push_back(
			PacketFactory::CreatePacketToSend(
				packetData, true, 5, nullptr
			)
		);
		return;
	}

	if (payload.entityData.deleteFlag)
	{
		for (auto& it : m_mapUidToClientAppData)
		{
			PacketManagerData* clientData = &it.second.clientData;

			if (endpointOwner == clientData && !alsoOwner)
				continue;

			PacketData<PayloadEntityInfo> packetData = PacketFactory::MakePacketData(++clientData->lastIDSent, clientData->lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);
			clientData->toSend.push_back(
				PacketFactory::CreatePacketToSend(packetData, true, 5)
			);
		}
	}
	else
	{
		for (auto& it : m_mapUidToClientAppData)
		{
			PacketManagerData* clientData = &it.second.clientData;

			if (endpointOwner == clientData && !alsoOwner)
				continue;

			PacketData<PayloadEntityInfo> packetData = PacketFactory::MakePacketData(++clientData->lastIDSent, clientData->lastFrameIDSent, HeaderFlags::NONE, false, payload);
			clientData->toSend.push_back(
				PacketFactory::CreatePacketToSend(packetData, false)
			);
		}
	}
}

void Server::SendGiveEntityIdNetwork(PayloadGiveEntityNetworkId payload, PacketManagerData* endpointOwner)
{
	PacketData<PayloadGiveEntityNetworkId> packetData = PacketFactory::MakePacketData(++endpointOwner->lastIDSent, endpointOwner->lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);
	endpointOwner->toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 5, nullptr
		)
	);
}

void Server::SendCollide(PayloadCollide payload, PacketManagerData* endpoint)
{
	PacketData<PayloadCollide> packetData = PacketFactory::MakePacketData(++endpoint->lastIDSent, endpoint->lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);
	endpoint->toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 5, nullptr
		)
	);

	std::cout << "collide" << std::endl;
}

void Server::SendGameTimer(PayloadGameTimer payload)
{
	for (auto& it : m_mapUidToClientAppData)
	{
		PacketManagerData* clientData = &it.second.clientData;

		PacketData<PayloadGameTimer> packetData = PacketFactory::MakePacketData(++clientData->lastIDSent, clientData->lastFrameIDSent, HeaderFlags::NONE, false, payload);
		clientData->toSend.push_back(
			PacketFactory::CreatePacketToSend(packetData, true, 5)
		);
	}
}

void Server::Update()
{
	m_deltaTime = m_clock.GetDeltaTime();
	UpdateFrameId();
	TryPing();
	ProcessIncomingPacket();
	SendDataClient();
	DeleteDeadClients();
}


void Server::Run()
{
	StartReceiveThread();
}

void Server::UpdateFrameId()
{
	for (auto& it : m_mapUidToClientAppData)
	{
		it.second.clientData.lastFrameIDSent++;
	}
}


void Server::HandlePacket(const Packet& packet)
{
	if (packet.data.size() < sizeof(PacketHeader))
		return;

	PacketHeader header;
	std::memcpy(&header, packet.data.data(), sizeof(header));
	PayloadType payloadType = static_cast<PayloadType>(header.payloadType);

	ClientAppData* clientAppData = GetClientAppData(packet.from);
	if (clientAppData == nullptr)
	{
		if (payloadType != PayloadType::CONNECT)
			return;
		const sockaddr_in& addr = packet.from;
		std::cout << "connected client" << std::endl;
		m_mapIpToUid[{addr.sin_addr.S_un.S_addr, addr.sin_port}] = ++m_lastUID;
		clientAppData = &m_mapUidToClientAppData[m_lastUID];
		clientAppData->clientData.addr = addr;
	}

	PacketManagerData& clientData = clientAppData->clientData;

	bool alwaysProcess = static_cast<HeaderFlags>(header.flag & 0x01) == HeaderFlags::ALWAYS_PROCESS;
	if (alwaysProcess && clientData.alwaysProccedIds.contains(header.id))
		return;
	else if (alwaysProcess)
		clientData.alwaysProccedIds.insert(header.id);


	bool isNewer = static_cast<int32_t>(header.id - clientData.lastIDReceived) > 0;
	bool isSameOrNewerFrame = static_cast<int32_t>(header.frameId - clientData.lastFrameIDReceived) >= 0;

	if (isNewer)
	{
		if (clientData.lastIDReceived > header.id)
			clientData.alwaysProccedIds.clear();
		clientData.lastIDReceived = header.id;
	}

	if (isSameOrNewerFrame || alwaysProcess)
	{
		if (isSameOrNewerFrame)
			clientData.lastFrameIDReceived = header.frameId;

		switch (payloadType)
		{
		case PayloadType::CONNECT: ProcessTypedPacket<PayloadConnect, Server>(packet, clientData, this, &Server::HandlePayloadConnect); break;
		case PayloadType::ACK: ProcessTypedPacket<PayloadACK, Server>(packet, clientData, this, &Server::HandleReceivePacketACK); break;
		case PayloadType::NAME: ProcessTypedPacket<PayloadName, Server>(packet, clientData, this, &Server::HandlePayloadName); break;
		case PayloadType::ENTITY_INFO: ProcessTypedPacket<PayloadEntityInfo, Server>(packet, clientData, this, &Server::HandlePayloadEntityInfo); break;
		case PayloadType::PLAYER_DEATH: ProcessTypedPacket<PayloadPlayerDeath, Server>(packet, clientData, this, &Server::HandlePayloadPlayerDeath); break;
		}
	}
}

void Server::DeleteDeadClients()
{
	std::vector<uint8_t> deadClients;
	for (auto& it : m_mapUidToClientAppData)
	{
		ClientAppData& clientAppData = it.second;
		if (clientAppData.isDead)
		{
			SendDisconnected(clientAppData.name);
			deadClients.push_back(it.first);
		}
	}

	for (size_t i = 0; i < deadClients.size(); i++)
	{
		uint8_t uid = deadClients[i];
		ClientAppData& clientAppData = m_mapUidToClientAppData[uid];
		sockaddr_in& addr = clientAppData.clientData.addr;
		App::GetInstance().GetGameManager().RemoveClient(&clientAppData.clientData);
		m_mapIpToUid.erase({ addr.sin_addr.S_un.S_addr, addr.sin_port });
		m_mapUidToClientAppData.erase(uid);
	}
}

void Server::SendDataClient()
{
	for (auto& it : m_mapUidToClientAppData)
	{
		PacketManagerData& dataClient = it.second.clientData;
		SendPacket(dataClient);
	}
}

void Server::SendConnected(char name[16])
{
	const char* a = " CONNECTED";
	char buffer[32];
	strncpy_s(buffer, name, sizeof(buffer));
	strncat_s(buffer, a, sizeof(buffer));
	SendMessageChat(buffer);
}

void Server::SendDisconnected(char name[16])
{
	const char* a = " DISCONNECTED";
	char buffer[32];
	strncpy_s(buffer, name, sizeof(buffer));
	strncat_s(buffer, a, sizeof(buffer));
	SendMessageChat(buffer);
}

void Server::SendDeath(char name[16])
{
	const char* a = " WAS KILLED";
	char buffer[32];
	strncpy_s(buffer, name, sizeof(buffer));
	strncat_s(buffer, a, sizeof(buffer));
	SendMessageChat(buffer);
}

void Server::SendMessageChat(char message[32])
{
	for (auto& it : m_mapUidToClientAppData)
	{
		PacketManagerData* clientData = &it.second.clientData;

		PayloadChat payloadToSend;
		strcpy_s(payloadToSend.message, message);
		PacketData<PayloadChat> packetData = PacketFactory::MakePacketData(++clientData->lastIDSent, clientData->lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, payloadToSend);
		clientData->toSend.push_back(
			PacketFactory::CreatePacketToSend(
				packetData, true, 6, nullptr
			)
		);
	}
}

void Server::TryPing()
{
	for (auto& it : m_mapUidToClientAppData)
	{
		ClientAppData& clientAppData = it.second;
		clientAppData.pingTime += m_deltaTime;

		if (clientAppData.pingTime < m_basePingTime)
			continue;

		clientAppData.pingTime = 0.f;

		PacketManagerData& clientData = it.second.clientData;
		PacketData<PayloadPing> packetData = PacketFactory::MakePacketData(++clientData.lastIDSent, clientData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, PayloadPing());
		clientData.toSend.push_back(
			PacketFactory::CreatePacketToSend(
				packetData, true, 6, std::bind(&Server::OnCompletePing, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
			)
		);
	}
}

void Server::OnCompletePing(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& clientData)
{
	switch (packetStatus)
	{
	case PacketStatus::Expired: {
		ClientAppData* clientAppData = GetClientAppData(clientData.addr);
		clientAppData->isDead = true;
		std::cout << "client dead" << std::endl;
		break;
	}
	case PacketStatus::Acknowledged:
		break;
	default:
		break;
	}
}

void Server::HandlePayloadConnect(PayloadConnect& payload, PacketManagerData& client)
{

}

void Server::HandlePayloadName(PayloadName& payload, PacketManagerData& client)
{
	PayloadComfirmName payloadToSend = { true };
	PacketData<PayloadComfirmName> packetData = PacketFactory::MakePacketData(++client.lastIDSent, client.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, true, payloadToSend);
	client.toSend.push_back(
		PacketFactory::CreatePacketToSend(
			packetData, true, 6, nullptr
		)
	);

	ClientAppData* clientData = GetClientAppData(client.addr);
	strcpy_s(clientData->name, payload.name);
	SendConnected(payload.name);

	GameManager& gameManager = App::GetInstance().GetGameManager();
	gameManager.NewPlayerProtocol(&client);
}

void Server::HandlePayloadEntityInfo(PayloadEntityInfo& payload, PacketManagerData& client)
{
	App::GetInstance().GetGameManager().HandlePayloadEntityInfo(payload, &client);
}

void Server::HandlePayloadPlayerDeath(PayloadPlayerDeath& payload, PacketManagerData& client)
{
	std::cout << " Player Death Handle " << std::endl;
	App::GetInstance().GetGameManager().DeletePlayerProtocol(payload,&client);
}

ClientAppData* Server::GetClientAppData(const sockaddr_in& addr)
{
	PairKey<ULONG, USHORT> key = { addr.sin_addr.S_un.S_addr,addr.sin_port };
	auto it = m_mapIpToUid.find(key);
	if (it != m_mapIpToUid.end())
	{
		uint32_t& uid = it->second;
		return &m_mapUidToClientAppData[uid];
	}
	return nullptr;
}

