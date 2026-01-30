#pragma once
#include <LibReseau.h>
#include "App.h"

class Client : public PacketManager
{
public:
    Client();
    ~Client();

    void ConnectTo(PCSTR ip, uint16_t port);
    void SendName(std::string name);
    void AddEntity(PayloadEntityInfo payload);
    void SendEntityInfo(PayloadEntityInfo payload);
    void SendPlayerDeath(uint32_t playerId, uint32_t killerId);

    bool IsClientRunning();
    bool IsConnectedToServer();

    void Update();

private:
    void UpdateServerTimeout();
    void UpdateFrameId();

    void Run() override;
    void Stop();
    void HandlePacket(const Packet& packet) override;

    void HandlePacketPing(PayloadPing& payload, PacketManagerData& endpointData); 
    void HandlePacketComfirmName(PayloadComfirmName& payload, PacketManagerData& endpointData);
    void HandlePacketEntityInfo(PayloadEntityInfo& payload, PacketManagerData& endpointData);
    void HandlePacketGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload, PacketManagerData& enpointData);
    void HandlePacketCollide(PayloadCollide& payload, PacketManagerData& endpointData);
    void HandlePacketChat(PayloadChat& payload, PacketManagerData& endpointData);
    void HandlePacketGameTimer(PayloadGameTimer& payload, PacketManagerData& endpointData);

    void OnCompleteConnected(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData);
    void OnCompleteName(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData);
    void OnCompleteAddEntity(PacketToSend& packet, PacketStatus packetStatus, PacketManagerData& serverData);

private:
    PacketManagerData m_serverData;
    bool m_connectedToServer = false;
    bool m_clientRunning = false;
    float m_serverTimeoutBaseTimer = 4.f;
    float m_serverTimeoutTimer = 0.f;
};