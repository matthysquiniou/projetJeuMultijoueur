#pragma once
#include "MultiEntity.h"
#include "EngineState.h"
#include "HelmState.h"
#include "UIWrapper.h"

class Player : public MultiEntity
{
public:
	Player();
	~Player();

	void HelmGoRight();
	void HelmGoLeft();

	void EngineGoForward();
	void EngineGoBack();

	void Shoot();

	void Update() override;

	void TakeDamage();
	void Repulse();

	bool IsAlive();

private:
	void CanonFollowCamera();
	void GetToTargetSpeed();
	void GetToTargetHelmTheta();
	void Reload();
	void UIReload();
	void MoveArrowPower();
	void MoveArrowDirection();
	void UpdateHealthUI();

private:
	EngineState m_engineState = EngineState::STOP;
	float m_maxSpeed = 12.f;
	float m_minSpeed = -3.f;
	float m_targetSpeed = 0.f;
	float m_currentSpeed = 0.f;
	float m_speedChangeFactor = 5.f;
	
	HelmState m_helmState = HelmState::FORWARD;
	float m_maxHelmTheta = DirectX::XM_PIDIV4;
	float m_targetHelmTheta = 0.f;
	float m_currentHelmTheta = 0.f;
	float m_helmThetaChangeFactor = 0.4f;

	bool m_canShoot = false;
	float m_baseReloadTime = 2.f;
	float m_currentReloadTime = 0.f;

	int m_baseH = 0;
	int m_baseW = 0;
	int m_dirPosCenter = 0;

	int m_baseHPow = 0;
	int m_baseWPow = 0;
	int m_enginePosCenter = 0;

	int m_maxHealth = 6;
	int m_health = 6;
};