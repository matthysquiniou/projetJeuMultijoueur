#pragma once
#include <unordered_map>
#include <queue>
#include <LibReseau.h>
#include <unordered_set>
#include "ClientAppData.h"

class Server : public PacketManager
{
public:
	Server();
	~Server();

	void SendEntityInfo(PayloadEntityInfo payload, PacketManagerData* endpointOwner, bool alsoOwner = false, bool onlyOwner = false);
	void SendGiveEntityIdNetwork(PayloadGiveEntityNetworkId payload, PacketManagerData* endpointOwner);
	void SendCollide(PayloadCollide payload, PacketManagerData* endpoint);
	void SendGameTimer(PayloadGameTimer payload);

	void Update();
	void Run() override;

private:
	void UpdateFrameId();

	void HandlePacket(const Packet& packet) override;
	void DeleteDeadClients();
	void SendDataClient();
	void SendConnected(char name[16]);
	void SendDisconnected(char name[16]);
	void SendDeath(char name[16]);
	void SendMessageChat(char message[32]);

	void TryPing();

	void OnCompletePing(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData);
	void HandlePayloadConnect(PayloadConnect& payload, PacketManagerData& client);
	void HandlePayloadName(PayloadName& payload, PacketManagerData& client);
	void HandlePayloadEntityInfo(PayloadEntityInfo& payload, PacketManagerData& client);
	void HandlePayloadPlayerDeath(PayloadPlayerDeath& payload, PacketManagerData& client);

	ClientAppData* GetClientAppData(const sockaddr_in& addr);

private:
	std::unordered_map<PairKey<ULONG, USHORT>, uint32_t, PairKeyHash<ULONG, USHORT>, PairKeyEqual<ULONG, USHORT>> m_mapIpToUid;
	std::unordered_map<uint32_t, ClientAppData> m_mapUidToClientAppData;

	uint32_t m_lastUID = 0;
	float m_basePingTime = 10.f;
};

