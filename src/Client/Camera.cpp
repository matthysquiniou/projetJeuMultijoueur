#include "Camera.h"
#include "Util.h"
#include <MultiEntity.h>
#include "App.h"

CameraMode Camera::m_cameraMode = CameraMode::FOLLOW_PLAYER;

float Camera::m_offsetY = 2.0f;
float Camera::m_maxDistance = 10.f;
float Camera::m_distance = 10.f;
float Camera::m_currentYtheta = 0.f;
float Camera::m_currentXZtheta = 0.f;
float Camera::m_maxPitch = DirectX::XM_PI * 0.125f;

void Camera::SetEntityToFollow(cpu_entity* entity)
{
    cpu_camera* camera = cpuEngine.GetCamera();
    const DirectX::XMFLOAT3& targetPos = entity->transform.pos;

    DirectX::XMVECTOR initialOffset = DirectX::XMVectorSet(0.f, 0.f, -m_maxDistance, 0.f);
    DirectX::XMVECTOR camPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&targetPos), initialOffset);
    DirectX::XMStoreFloat3(&camera->transform.pos, camPos);

    m_distance = m_maxDistance;
    m_currentYtheta = 0.f;
    m_currentXZtheta = 0.f;
}

void Camera::UpdateRotation(DirectX::XMFLOAT2 mouseDelta)
{
    cpu_entity* entity = GetEntityToFollow();

    if (entity == nullptr)
        return;

    cpu_camera* camera = cpuEngine.GetCamera();
    const DirectX::XMFLOAT3& targetPos = entity->transform.pos;

    m_currentYtheta += mouseDelta.x * 0.01f;
    m_currentXZtheta += mouseDelta.y * 0.01f;

    m_currentXZtheta = std::clamp(m_currentXZtheta, -m_maxPitch, m_maxPitch);

    DirectX::XMMATRIX yaw = DirectX::XMMatrixRotationY(m_currentYtheta);
    DirectX::XMMATRIX pitch = DirectX::XMMatrixRotationX(m_currentXZtheta);
    DirectX::XMMATRIX rot = pitch * yaw;

    if (camera->transform.pos.y > 0.f && m_distance < m_maxDistance)
    {
        m_distance = std::min(m_distance + 0.05f, m_maxDistance);
    }

    DirectX::XMVECTOR offset = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.f, 0.f, -m_distance, 0.f), rot);

    DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&targetPos), offset);

    DirectX::XMFLOAT3 finalPos;
    DirectX::XMStoreFloat3(&finalPos, newPos);

    if (finalPos.y < 0.f)
        finalPos.y = 0.f;

    camera->transform.pos = finalPos;
    camera->transform.LookAt(targetPos.x, targetPos.y + m_offsetY, targetPos.z);
}

void Camera::SwitchCameraMode(CameraMode cameraMode)
{
    m_cameraMode = cameraMode;

    cpu_entity* entity = GetEntityToFollow();
    if (entity != nullptr)
        SetEntityToFollow(entity);
}

cpu_entity* Camera::GetEntityToFollow()
{


    switch (m_cameraMode)
    {
    case CameraMode::FOLLOW_PLAYER:
    {
        MultiEntity* entity = App::GetInstance().GetSceneManager().GetFirstMultiEntityOfTag(EntityTag::ACTIVE_PLAYER);
        if (entity == nullptr)
            return nullptr;
        return entity->GetEntity(EntityType::HULL)->GetCpuEntity();
        break;
    }
       
    case CameraMode::FOLLOW_LAST_BULLET:
    {
        MultiEntity* entity = App::GetInstance().GetSceneManager().GetFirstMultiEntityOfTag(EntityTag::LAST_PROJECTILLE_SHOT);
        if (entity == nullptr)
            return nullptr;
        return entity->GetEntity(EntityType::PROJECTILLE)->GetCpuEntity();
        break;
    }
    default:
        return nullptr;
        break;
    }
}
