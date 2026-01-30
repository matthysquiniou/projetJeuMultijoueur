#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

class LibLoader
{
public:
	static void Loadlib();
	static void CleanLib();
};

