#pragma once
#include "EntityWrapper.h"
#include "CameraMode.h"

class Camera
{
public:
	static void UpdateRotation(DirectX::XMFLOAT2 mouseDelta);
	static void SwitchCameraMode(CameraMode cameraMode);

private:
	static cpu_entity* GetEntityToFollow();
	static void SetEntityToFollow(cpu_entity* entity);

private:
	static CameraMode m_cameraMode;

	static float m_offsetY;
	static float m_maxDistance;
	static float m_distance;
	static float m_currentYtheta;
	static float m_currentXZtheta;
	static float m_maxPitch;
};

