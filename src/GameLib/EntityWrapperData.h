#pragma once
#include "DirectXMath.h"
#include "EntityType.h"

struct EntityWrapperData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 lookTo;
	EntityType entityType;
};
