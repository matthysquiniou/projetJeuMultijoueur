#pragma once
#include "DirectXMath.h"

class Util
{
public:
	static DirectX::XMFLOAT3 AddXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	static DirectX::XMFLOAT3 MinusXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	static DirectX::XMFLOAT3 CrossXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	static float EuclidieanDistance(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	static void NormalizeXMFLOAT3(DirectX::XMFLOAT3& v);
	static void SetCursorPositionCenter();
	static void SetCursorVisible(bool visible);
	static DirectX::XMFLOAT2 GetCursorDelta();
	static void ResetCursorDelta();
private:
	static DirectX::XMFLOAT2 m_lastCursorPos;
};
