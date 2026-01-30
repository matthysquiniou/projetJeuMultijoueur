#include "WrapperInput.h"

int WrapperInput::GetKey() {
	int keycode;
	for (int i = 0; i < 255; i++)
	{
		if (cpuInput.IsKeyDown(i)) {
			return keycode = i;
		}
	}
	return -1;
}

bool WrapperInput::IsKey(ControllerBind key)
{
	return cpuInput.IsKey(static_cast<int>(key));
}

bool WrapperInput::IsKeyDown(ControllerBind key)
{
	return cpuInput.IsKeyDown(static_cast<int>(key));
}

bool WrapperInput::IsKeyUp(ControllerBind key)
{
	return cpuInput.IsKeyUp(static_cast<int>(key));
}
