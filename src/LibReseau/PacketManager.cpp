#include "pch.h"
#include "PacketManager.h"
#include <iostream>
#include "Benchmark.h"

PacketManager::PacketManager()
{
	InitializeCriticalSection(&m_incomingCS);
	InitializeCriticalSection(&m_runningCS);
}

PacketManager::~PacketManager()
{
	EndReceiveThread();
	DeleteCriticalSection(&m_incomingCS);
	DeleteCriticalSection(&m_runningCS);
}


void PacketManager::ProcessIncomingPacket()
{
	std::queue<Packet> packets;
	EnterCriticalSection(&m_incomingCS);
	std::swap(packets, m_incomingPackets);
	LeaveCriticalSection(&m_incomingCS);

	while (!packets.empty())
	{
		HandlePacket(packets.front());
		packets.pop();
	}
}

void PacketManager::SendPacket(PacketManagerData& endpointData)
{
	std::vector<PacketToSend>& toSend = endpointData.toSend;

	for (size_t i = 0; i < toSend.size();)
	{
		PacketToSend& packet = toSend[i];
		if (!packet.replicate)
		{
			m_socket.SendTo(packet, packet.data.size(), endpointData.addr);

			std::swap(toSend[i], toSend.back());
			toSend.pop_back();
			continue;
		}

		if (packet.replicateTimeout >= packet.replicateBaseTimeout)
		{
			if (packet.onComplete)
			{
				packet.onComplete(packet, PacketStatus::Expired, endpointData);
			}
			std::swap(toSend[i], toSend.back());
			toSend.pop_back();
			continue;
		}

		packet.replicateTimeout += m_deltaTime;

		if (packet.replicateCount < packet.maxReplicateCount)
		{
			packet.replicateTimer += m_deltaTime;
			if (packet.replicateTimer >= packet.replicateBaseTimer)
			{
				m_socket.SendTo(packet, packet.data.size(), endpointData.addr);

				packet.replicateCount++;
				packet.replicateTimer = 0.f;
			}
		}

		++i;
	}
}

void PacketManager::HandleReceivePacketACK(PayloadACK& payload, PacketManagerData& endpointData)
{
	std::vector<PacketToSend>& toSend = endpointData.toSend;

	for (size_t i = 0; i < toSend.size(); ++i)
	{
		PacketToSend& packet = toSend[i];
		if (packet.packetID == payload.packetToACKID)
		{
			if (packet.onComplete)
				packet.onComplete(packet, PacketStatus::Acknowledged, endpointData);

			toSend[i] = std::move(toSend.back());
			toSend.pop_back();
			break;
		}
	}
}

void PacketManager::StartReceiveThread()
{
	EnterCriticalSection(&m_runningCS);
	m_running = true;
	LeaveCriticalSection(&m_runningCS);

	m_receivePacketThread.Start(ReceivePacketThreadFunc, this);
}

void PacketManager::EndReceiveThread()
{
	EnterCriticalSection(&m_runningCS);
	m_running = false;
	LeaveCriticalSection(&m_runningCS);

	m_receivePacketThread.Stop();
}

DWORD __stdcall PacketManager::ReceivePacketThreadFunc(LPVOID lpParameter)
{
	PacketManager* packetManager = (PacketManager*)(lpParameter);

	while (true)
	{
		EnterCriticalSection(&packetManager->m_runningCS);
		bool running = packetManager->m_running;
		LeaveCriticalSection(&packetManager->m_runningCS);

		if (!running)
			break;

		Packet packet = packetManager->m_socket.ReceiveFrom();

		if (packet.data.size() == 0)
			continue;

		EnterCriticalSection(&packetManager->m_incomingCS);
		packetManager->m_incomingPackets.push(std::move(packet)); // move to avoid data copy
		LeaveCriticalSection(&packetManager->m_incomingCS);
	}

	return 0;
}
