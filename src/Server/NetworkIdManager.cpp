#include "NetworkIdManager.h"

uint32_t NetworkIdManager::m_lastId = 0;

uint32_t NetworkIdManager::GetNewId()
{
	uint32_t id = m_lastId++;
	return id;
}

void NetworkIdManager::ResetIds()
{
	m_lastId = 0;
}