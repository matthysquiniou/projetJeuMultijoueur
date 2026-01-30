#include "Player.h"
#include "App.h"
#include "Projectille.h"
#include "SceneGameplay.h"
#include "Camera.h"

Player::Player() : MultiEntity()
{
	AddEntity(EntityType::CANON);
	AddEntity(EntityType::HULL);
}

Player::~Player()
{
}

void Player::HelmGoRight()
{
	switch (m_helmState)
	{
	case HelmState::LEFT:
		m_helmState = HelmState::HALF_LEFT;
		m_targetHelmTheta = -m_maxHelmTheta * 0.5f;
		break;
	case HelmState::HALF_LEFT:
		m_helmState = HelmState::FORWARD;
		m_targetHelmTheta = 0.f;
		break;
	case HelmState::FORWARD:
		m_helmState = HelmState::HALF_RIGHT;
		m_targetHelmTheta = m_maxHelmTheta * 0.5f;
		break;
	case HelmState::HALF_RIGHT:
		m_helmState = HelmState::RIGHT;
		m_targetHelmTheta = m_maxHelmTheta;
		break;
	default:
		break;
	}
}

void Player::HelmGoLeft()
{
	switch (m_helmState)
	{
	case HelmState::HALF_LEFT:
		m_helmState = HelmState::LEFT;
		m_targetHelmTheta = -m_maxHelmTheta;
		break;
	case HelmState::FORWARD:
		m_helmState = HelmState::HALF_LEFT;
		m_targetHelmTheta = -m_maxHelmTheta * 0.5f;
		break;
	case HelmState::HALF_RIGHT:
		m_helmState = HelmState::FORWARD;
		m_targetHelmTheta = 0.f;
		break;
	case HelmState::RIGHT:
		m_helmState = HelmState::HALF_RIGHT;
		m_targetHelmTheta = m_maxHelmTheta * 0.5f;
		break;
	default:
		break;
	}
}

void Player::EngineGoForward()
{
	switch (m_engineState)
	{
	case EngineState::BACK:
		m_engineState = EngineState::STOP;
		m_targetSpeed = 0.f;
		break;
	case EngineState::STOP:
		m_engineState = EngineState::ONE_FOURTH;
		m_targetSpeed = m_maxSpeed * 0.25;
		break;
	case EngineState::ONE_FOURTH:
		m_engineState = EngineState::HALF;
		m_targetSpeed = m_maxSpeed * 0.5f;
		break;
	case EngineState::HALF:
		m_engineState = EngineState::THREE_FOURTH;
		m_targetSpeed = m_maxSpeed * 0.75f;
		break;
	case EngineState::THREE_FOURTH:
		m_engineState = EngineState::FULL;
		m_targetSpeed = m_maxSpeed;
		break;
	default:
		break;
	}
}

void Player::EngineGoBack()
{
	switch (m_engineState)
	{
	case EngineState::STOP:
		m_engineState = EngineState::BACK;
		m_targetSpeed = m_minSpeed;
		break;
	case EngineState::ONE_FOURTH:
		m_engineState = EngineState::STOP;
		m_targetSpeed = 0.f;
		break;
	case EngineState::HALF:
		m_engineState = EngineState::ONE_FOURTH;
		m_targetSpeed = m_maxSpeed * 0.25f;
		break;
	case EngineState::THREE_FOURTH:
		m_engineState = EngineState::HALF;
		m_targetSpeed = m_maxSpeed * 0.5f;
		break;
	case EngineState::FULL:
		m_engineState = EngineState::THREE_FOURTH;
		m_targetSpeed = m_maxSpeed * 0.75f;
		break;
	default:
		break;
	}
}

void Player::Shoot()
{
	if (m_canShoot)
	{
		MultiEntity* oldProjectille = App::GetInstance().GetSceneManager().GetFirstMultiEntityOfTag(EntityTag::LAST_PROJECTILLE_SHOT);
		if (oldProjectille != nullptr)
			oldProjectille->RemoveTag(EntityTag::LAST_PROJECTILLE_SHOT);

		m_canShoot = false;
		cpu_camera* camera = cpuEngine.GetCamera();
		DirectX::XMFLOAT3 shootDir = camera->transform.dir;
		shootDir.y *= 1.5f;
		if (shootDir.y < 0) shootDir.y = 0;
		EntityWrapper* canon = GetEntity(EntityType::CANON);
		Projectille* projectille = new Projectille();
		projectille->SetPosition({ canon->GetPosition().x,canon->GetPosition().y +1 ,canon->GetPosition().z });
		projectille->GetEntity(EntityType::PROJECTILLE)->LookTo(shootDir);
		projectille->AddTags({ EntityTag::CLIENT_OWNED_ENTITY,EntityTag::LAST_PROJECTILLE_SHOT });
		App::GetInstance().GetSceneManager().AddEntityToScene<SceneGameplay>(projectille);
	}
}

void Player::Update()
{
    if (HasTag(EntityTag::ACTIVE_PLAYER))
    {
        GetToTargetSpeed();
        GetToTargetHelmTheta();
        Reload();
        EntityWrapper* canon = GetEntity(EntityType::CANON);
        EntityWrapper* hull = GetEntity(EntityType::HULL);
        canon->LookTo(hull->GetCpuEntity()->transform.dir);
        Move(m_currentSpeed * cpuTime.delta);
        AddYPR(m_currentHelmTheta * cpuTime.delta);
        CanonFollowCamera();
		MoveArrowPower();
		MoveArrowDirection();
		UpdateHealthUI();
    }
}

void Player::TakeDamage()
{
    m_health--;
}

void Player::Repulse()
{
    TakeDamage();
    m_currentSpeed *= -0.4f;
    m_engineState = EngineState::STOP;
    m_targetSpeed = 0.f;
    m_helmState = HelmState::FORWARD;
    m_targetHelmTheta = 0.f;

    EntityWrapper* canon = GetEntity(EntityType::CANON);
    EntityWrapper* hull = GetEntity(EntityType::HULL);
    canon->LookTo(hull->GetCpuEntity()->transform.dir);
    Move(0.1f * m_currentSpeed);
    CanonFollowCamera();
}

bool Player::IsAlive()
{
	return m_health > 0;
}

void Player::CanonFollowCamera()
{
	cpu_camera* camera = cpuEngine.GetCamera();
	EntityWrapper* canon = GetEntity(EntityType::CANON);
	DirectX::XMFLOAT3 newDir = camera->transform.dir;
	newDir.y = canon->GetCpuEntity()->transform.pos.y;
	canon->GetCpuEntity()->transform.LookTo(newDir);
}

void Player::GetToTargetSpeed()
{
	float maxDelta = m_speedChangeFactor * cpuTime.delta;
	float delta = m_targetSpeed - m_currentSpeed;

	if (std::abs(delta) <= maxDelta)
		m_currentSpeed = m_targetSpeed;
	else
		m_currentSpeed += (delta > 0.f ? maxDelta : -maxDelta);
}

void Player::GetToTargetHelmTheta()
{
	float maxDelta = m_helmThetaChangeFactor * cpuTime.delta;
	float delta = m_targetHelmTheta - m_currentHelmTheta;

	if (std::abs(delta) <= maxDelta)
		m_currentHelmTheta = m_targetHelmTheta;
	else
		m_currentHelmTheta += (delta > 0.f ? maxDelta : -maxDelta);
}

void Player::Reload()
{
	if (!m_canShoot)
	{
		m_currentReloadTime += cpuTime.delta;
		if (m_currentReloadTime >= m_baseReloadTime) {
			m_canShoot = true;
			m_currentReloadTime = 0.f;
		}
		else {
			UIReload();
		}
	}
}

void Player::UIReload()
{
	UIWrapper* pBullet;
	Scene* m_pScene = App::GetInstance().GetSceneManager().GetScene<SceneGameplay>();
	pBullet = m_pScene->GetUI(EntityType::BULLET);	
	if (m_currentReloadTime > m_baseReloadTime) {
		pBullet->SetHeight(pBullet, 23 /*base bullet.png height */);
	}
	else {
		pBullet->SetHeight(pBullet, (m_currentReloadTime * (1 / m_baseReloadTime)) * 23 /*base bullet.png height */);
	}
}

void Player::MoveArrowDirection()
{
	UIWrapper* pArrowUI;
	UIWrapper* pDirUI;
	Scene* pScene = App::GetInstance().GetSceneManager().GetScene<SceneGameplay>();

	if (pScene == nullptr) return;
	pArrowUI = pScene->GetUI(EntityType::ARROW_DOWN);
	pDirUI = pScene->GetUI(EntityType::ENGINE_DIRECTION);

	if (pArrowUI == nullptr || pDirUI == nullptr) return;
	m_baseH = pArrowUI->m_pSprite->y;
	m_baseW = pDirUI->m_pTexture->width;
	m_dirPosCenter = pDirUI->m_pSprite->x;

	pArrowUI->UIpositionPixels(m_baseH, m_dirPosCenter + (m_currentHelmTheta * (m_baseW * 0.5f)));
}

void Player::UpdateHealthUI()
{
	Scene* pScene = App::GetInstance().GetSceneManager().GetScene<SceneGameplay>();
	if (pScene == nullptr) return;
	switch (m_health) {
	case 6:
		pScene->GetUI(EntityType::HEALTH6)->SetActive(true);
		pScene->GetUI(EntityType::HEALTH5)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH4)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH3)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH2)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH1)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH0)->SetActive(false);
		break;
	case 5:
		pScene->GetUI(EntityType::HEALTH6)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH5)->SetActive(true);
		break;
	case 4:
		pScene->GetUI(EntityType::HEALTH5)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH4)->SetActive(true);
		break;
	case 3:
		pScene->GetUI(EntityType::HEALTH4)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH3)->SetActive(true);
		break;
	case 2:
		pScene->GetUI(EntityType::HEALTH3)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH2)->SetActive(true);
		break;
	case 1:
		pScene->GetUI(EntityType::HEALTH2)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH1)->SetActive(true);
		break;
	case 0:
		pScene->GetUI(EntityType::HEALTH1)->SetActive(false);
		pScene->GetUI(EntityType::HEALTH0)->SetActive(true);
		break;
	case -1:
		m_health = 6;
		break;
	}
}

void Player::MoveArrowPower()
{
	UIWrapper* pArrowPowerUI;
	UIWrapper* pEngineUI;
	Scene* pScene = App::GetInstance().GetSceneManager().GetScene<SceneGameplay>();

	if (pScene == nullptr) return;
	pArrowPowerUI = pScene->GetUI(EntityType::ARROW);
	pEngineUI = pScene->GetUI(EntityType::ENGINE_POWER);

	if (pArrowPowerUI == nullptr || pEngineUI == nullptr) return;
	m_baseHPow = pArrowPowerUI->m_pSprite->x;
	m_baseWPow = pEngineUI->m_pTexture->height;
	m_enginePosCenter = pEngineUI->m_pSprite->y;

	if (m_currentSpeed <= 0) {
		pArrowPowerUI->UIpositionPixels(m_enginePosCenter + (m_baseWPow * 0.4f) - (m_baseWPow * 0.2f) + (m_currentSpeed * ((m_baseWPow * 0.16) / m_minSpeed)), m_baseHPow);
	}
	else {
		pArrowPowerUI->UIpositionPixels(m_enginePosCenter + (m_baseWPow * 0.4f) - (m_baseWPow * 0.2f) - (m_currentSpeed * ((m_baseWPow * 0.16) / (m_maxSpeed * 0.25f)) * 0.9f), m_baseHPow);
	}
}




