#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
#include "Packet.h"

class UDPSocket
{
public:
	UDPSocket();
	~UDPSocket();

	sockaddr_in GetSockAddrIn();

	bool Bind(uint16_t port = 1888, ULONG mask = INADDR_ANY);

	bool SendTo(const Packet& buffer, uint16_t bufferSize, PCSTR ip, uint16_t port = 1888);
	bool SendTo(const Packet& buffer, uint16_t bufferSize, sockaddr_in to);
	bool SendTo(const uint8_t* buffer, uint16_t bufferSize, PCSTR ip, uint16_t port);
	bool SendTo(const uint8_t* buffer, uint16_t bufferSize, sockaddr_in to);
	bool SendTo(const std::string buffer, uint16_t bufferSize, PCSTR ip, uint16_t port = 1888);
	bool SendTo(const std::string buffer, uint16_t bufferSize, sockaddr_in to);
	bool SendTo(const char* buffer, uint16_t bufferSize, PCSTR ip, uint16_t port = 1888);
	bool SendTo(const char* buffer, uint16_t bufferSize, sockaddr_in to);

	Packet ReceiveFrom();

private:
	SOCKET m_socket;
	sockaddr_in m_addr;
};

