#include "Util.h"
#include <cpu-engine.h>

DirectX::XMFLOAT2 Util::m_lastCursorPos;

DirectX::XMFLOAT3 Util::AddXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
    DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR vc = DirectX::XMVectorAdd(va, vb);

    DirectX::XMFLOAT3 v3;
    DirectX::XMStoreFloat3(&v3, vc);
    return v3;
}

DirectX::XMFLOAT3 Util::MinusXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
    DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR vc = DirectX::XMVectorSubtract(va, vb);

    DirectX::XMFLOAT3 v3;
    DirectX::XMStoreFloat3(&v3, vc);
    return v3;
}

DirectX::XMFLOAT3 Util::CrossXMFLOAT3(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
    DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR vc = DirectX::XMVector3Cross(va, vb);

    DirectX::XMFLOAT3 v3;
    DirectX::XMStoreFloat3(&v3, vc);
    return v3;
}

float Util::EuclidieanDistance(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
    DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR d = DirectX::XMVectorSubtract(a, b);

    return DirectX::XMVectorGetX(DirectX::XMVector3Length(d));
}

void Util::NormalizeXMFLOAT3(DirectX::XMFLOAT3& v)
{
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&v);
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&v, vec);
}

void Util::SetCursorPositionCenter()
{
    cpu_window* window = cpuEngine.GetWindow();
    POINT p = { window->GetWidth() / 2, window->GetHeight() / 2 };
    ClientToScreen(window->GetHWND(), &p);
    SetCursorPos(p.x, p.y);
}

void Util::SetCursorVisible(bool visible)
{
    if (visible)
    {
        while (::ShowCursor(TRUE) < 0) {}
    }
    else
    {
        while (::ShowCursor(FALSE) >= 0) {}
    }
}

DirectX::XMFLOAT2 Util::GetCursorDelta()
{
    DirectX::XMFLOAT2 currentPos;
    cpuEngine.GetCursor(currentPos);

    DirectX::XMFLOAT2 delta;
    delta.x = currentPos.x - m_lastCursorPos.x;
    delta.y = currentPos.y - m_lastCursorPos.y;

    m_lastCursorPos = currentPos;

    return delta;
}

void Util::ResetCursorDelta()
{
    cpuEngine.GetCursor(m_lastCursorPos);
}

