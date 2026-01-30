#pragma once
#include "ControllerBind.h"
#include "cpu-engine.h"

class WrapperInput
{
public:
	static int GetKey();
	static bool IsKey(ControllerBind key);
	static bool IsKeyDown(ControllerBind key);
	static bool IsKeyUp(ControllerBind key);
};
