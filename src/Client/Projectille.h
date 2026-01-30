#pragma once
#include "MultiEntity.h"

class Projectille : public MultiEntity
{
public:
	Projectille();
	~Projectille();

	void Update() override;
private:
	float m_velocity = 30.f;
	DirectX::XMFLOAT3 m_gravityDir = { 0.f,-1.f,0.f };
	float m_gravityStrength = 0.1f;
};

