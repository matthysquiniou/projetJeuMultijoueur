#pragma once
#include "EntityWrapperData.h"
#include "MultiEntityType.h"

struct NetworkEntityData 
{
	uint32_t networkId;
	uint32_t localId;
	EntityWrapperData entityWrapperDatas[2]; // game doesn't have more than two (for now)
	uint8_t wrapperCount;
	MultiEntityType multiEntityType = MultiEntityType::DEFAULT;
	bool deleteFlag = false;
	bool networkIdInit = false;
};
