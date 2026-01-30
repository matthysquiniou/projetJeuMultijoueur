#pragma once
#include <cstdint>
#include <vector>
#include <unordered_set>

class NetworkIdManager
{
public:
	static uint32_t GetNewId();
	static void ResetIds();
private:
	static uint32_t m_lastId;
};

