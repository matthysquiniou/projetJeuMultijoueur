#include "pch.h"
#include "LibLoader.h"

void LibLoader::Loadlib()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
}

void LibLoader::CleanLib()
{
	WSACleanup();
}