#pragma once
#include "GameObject.h"

class CTankEnemy : public CGameObject
{
public:
    CTankEnemy();
    virtual ~CTankEnemy();

    virtual void Animate(float fElapsedTime) override;
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

    void Move(XMFLOAT3 xmf3Shift);

    void ReverseDirection() { m_iMoveDirection = -m_iMoveDirection; }

    virtual int PickObjectByRayIntersection(XMFLOAT3& pickPosition, XMFLOAT4X4& viewMatrix, float* pfHitDistance) override;

    void SetTankParts(CGameObject* pLower, CGameObject* pUpper, CGameObject* pBarrel);

    virtual bool CheckCollisionWith(CGameObject* pOther);
protected:
    CGameObject* m_pLowerBody = nullptr;   // 하부
    CGameObject* m_pUpperBody = nullptr;   // 포탑
    CGameObject* m_pBarrel = nullptr;   // 포신

    XMFLOAT3 m_xmf3PrePos = { 0.0f, 0.0f, 0.0f };

    float m_fElapsedTime = 0.0f;
    float m_fRandomMoveTime = 1.0f;
    float m_fRotationAngle = 0.0f;
    int   m_iMoveDirection = 1;
};