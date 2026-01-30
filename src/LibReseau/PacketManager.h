#pragma once
#include <queue>
#include "PacketManagerData.h"
#include "PacketFactory.h"

class PacketManager
{
public:
	PacketManager();
	~PacketManager();

	void ProcessIncomingPacket();
	void SendPacket(PacketManagerData& endpointData);

	virtual void Run() = 0;

protected:

	void HandleReceivePacketACK(PayloadACK& packetData, PacketManagerData& endpointData);
	template<typename PayloadType>
	void HandleSendPacketACK(const PacketData<PayloadType>& packetData, PacketManagerData& endpointData)
	{
		if (!packetData.header.needACK)
			return;

		PayloadACK payload = { packetData.header.id };
		PacketData<PayloadACK> packetDataSend = PacketFactory::MakePacketData<PayloadACK>(++endpointData.lastIDSent, endpointData.lastFrameIDSent, HeaderFlags::ALWAYS_PROCESS, false, payload);
		endpointData.toSend.push_back(PacketFactory::CreatePacketToSend(packetDataSend, true, 3));
	}

	template<typename PayloadType, typename OWNER>
	void ProcessTypedPacket(const Packet& packet, PacketManagerData& endpointData, OWNER* owner, std::function<void(OWNER&, PayloadType&, PacketManagerData&)> HandlePayload)
	{
		PacketData<PayloadType> packedData;
		bool parsedPacket = PacketFactory::TryParsePaket(packet, packedData);
		if (!parsedPacket) return;
		
		HandleSendPacketACK<PayloadType>(packedData, endpointData);
		HandlePayload(*owner, packedData.data, endpointData);
	}

	virtual void HandlePacket(const Packet& packet) = 0;

	void StartReceiveThread();
	void EndReceiveThread();

	static DWORD WINAPI ReceivePacketThreadFunc(LPVOID lpParameter);

protected:
	UDPSocket m_socket;

	std::queue<Packet> m_incomingPackets;
	CRITICAL_SECTION m_incomingCS;

	bool m_running = false;
	CRITICAL_SECTION m_runningCS;

	ThreadWrapper m_receivePacketThread;

	Clock m_clock;
	float m_deltaTime = 0.f;
};

