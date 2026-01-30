#include "pch.h"
#include "UDPSocket.h"
#include <iostream>

UDPSocket::UDPSocket()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
		std::cerr << "Erreur creation socket : " << WSAGetLastError() << std::endl;		

	DWORD timeout = 100; 
	if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
	{
		std::cerr << "Erreur setsockopt SO_RCVTIMEO : " << WSAGetLastError() << std::endl;
	}
}

UDPSocket::~UDPSocket()
{
	closesocket(m_socket);
}

sockaddr_in UDPSocket::GetSockAddrIn()
{
	return m_addr;
}

bool UDPSocket::Bind(uint16_t port, ULONG mask)
{
	m_addr.sin_addr.s_addr = mask;
	m_addr.sin_port = htons(port);
	m_addr.sin_family = AF_INET;
	int res = bind(m_socket, (sockaddr*)(&m_addr), sizeof(m_addr));
	if (res != 0)
		return false;
	return true;
}

bool UDPSocket::SendTo(const Packet& buffer, uint16_t bufferSize, PCSTR ip, uint16_t port)
{
	return SendTo(buffer.data.data(), bufferSize, ip, port);
}

bool UDPSocket::SendTo(const Packet& buffer, uint16_t bufferSize, sockaddr_in to)
{
	return SendTo(buffer.data.data(), bufferSize, to);
}

bool UDPSocket::SendTo(const uint8_t* buffer, uint16_t bufferSize, PCSTR ip, uint16_t port)
{ 
	return SendTo((char*)buffer, bufferSize, ip, port);
}

bool UDPSocket::SendTo(const uint8_t* buffer, uint16_t bufferSize, sockaddr_in to)
{
	return SendTo((char*)buffer, bufferSize, to);
}

bool UDPSocket::SendTo(const std::string buffer, uint16_t bufferSize, PCSTR ip, uint16_t port)
{
	return SendTo(buffer.data(), bufferSize, ip, port);
}

bool UDPSocket::SendTo(const std::string buffer, uint16_t bufferSize, sockaddr_in to)
{
	return SendTo(buffer.data(), bufferSize, to);
}

bool UDPSocket::SendTo(const char* buffer, uint16_t bufferSize, PCSTR ip, uint16_t port)
{
	sockaddr_in dst;
	dst.sin_port = htons(port);
	dst.sin_family = AF_INET;

	if (inet_pton(AF_INET, ip, &dst.sin_addr) <= 0)
	{
		return false;
	}

	return SendTo(buffer, bufferSize, dst);
}

bool UDPSocket::SendTo(const char* buffer, uint16_t bufferSize, sockaddr_in to)
{
	int byteSent = sendto(m_socket, buffer, bufferSize, 0, (const sockaddr*)(&to), sizeof(to));
	if (byteSent == SOCKET_ERROR)
		return false;
	return true;
}

Packet UDPSocket::ReceiveFrom()
{
	sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	char receiveBuffer[1024];
	int ret = recvfrom(m_socket, receiveBuffer, 1024, 0, (sockaddr*)(&from), &fromlen);
	if (ret <= 0)
		return {};

	Packet packet;
	packet.data.resize(ret);
	std::memcpy(packet.data.data(), receiveBuffer, ret);
	packet.from = from;
	return packet;
}