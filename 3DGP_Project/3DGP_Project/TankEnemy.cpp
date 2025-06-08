#include "TankEnemy.h"

CTankEnemy::CTankEnemy()
{

}

CTankEnemy::~CTankEnemy()
{

}

void CTankEnemy::Move(XMFLOAT3 xmf3Shift)
{
    SetPosition(Vector3::Add(GetPosition(), xmf3Shift));
}

void CTankEnemy::Animate(float fElapsedTime)
{
    if (!m_bActive) return;

    SetPrePosition(GetPosition());

    m_fElapsedTime += fElapsedTime;

    if (m_fElapsedTime >= m_fRandomMoveTime)
    {
        m_fRotationAngle = static_cast<float>((rand() % 180) - 90); // -90 ~ +90µµ
        m_fElapsedTime = 0.0f;
        m_fRandomMoveTime = 1.0f + ((float)(rand() % 200) / 100.0f);
    }

    if (fabs(m_fRotationAngle) > 0.0f)
    {
        float rotateSpeed = 90.0f;
        float rotateAmount = rotateSpeed * fElapsedTime;

        if (m_fRotationAngle > 0.0f)
        {
            Rotate(0.0f, rotateAmount, 0.0f);
            m_fRotationAngle -= rotateAmount;
        }
        else
        {
            Rotate(0.0f, -rotateAmount, 0.0f);
            m_fRotationAngle += rotateAmount;
        }
    }

    MoveForward(m_iMoveDirection * 5.0f * fElapsedTime);

    if (m_pLowerBody)
    {
        m_pLowerBody->Animate(fElapsedTime, m_xmf4x4World);
    }

    if (m_pUpperBody)
    {
        m_pUpperBody->Animate(fElapsedTime, m_pLowerBody->GetWorldMAT());
    }

    if (m_pBarrel)
    {
        m_pBarrel->Animate(fElapsedTime, m_pUpperBody->GetWorldMAT());
    }
}

void CTankEnemy::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
    if (!m_bActive) return;

    if (m_pUpperBody)
    {
        m_pUpperBody->Render(pd3dCommandList, pCamera);
    }

    if (m_pLowerBody)
    {
        m_pLowerBody->Render(pd3dCommandList, pCamera);
    }

    if (m_pBarrel)
    {
        m_pBarrel->Render(pd3dCommandList, pCamera);
    }
}

int CTankEnemy::PickObjectByRayIntersection(XMFLOAT3& pickPosition, XMFLOAT4X4& viewMatrix, float* pfHitDistance)
{
    int nIntersected = 0;
    float fNearestHitDistance = FLT_MAX;

    float fHitDistance = FLT_MAX;

    if (m_pUpperBody)
    {
        if (m_pUpperBody->PickObjectByRayIntersection(pickPosition, viewMatrix, &fHitDistance))
        {
            fNearestHitDistance = fHitDistance;
            nIntersected = 1;
        }
    }

    if (m_pLowerBody)
    {
        if (m_pLowerBody->PickObjectByRayIntersection(pickPosition, viewMatrix, &fHitDistance))
        {
            if (!nIntersected || fHitDistance < fNearestHitDistance)
            {
                fNearestHitDistance = fHitDistance;
                nIntersected = 1;
            }
        }
    }

    if (m_pBarrel)
    {
        if (m_pBarrel->PickObjectByRayIntersection(pickPosition, viewMatrix, &fHitDistance))
        {
            if (!nIntersected || fHitDistance < fNearestHitDistance)
            {
                fNearestHitDistance = fHitDistance;
                nIntersected = 1;
            }
        }
    }

    if (nIntersected && pfHitDistance) *pfHitDistance = fNearestHitDistance;
    return nIntersected;
}



void CTankEnemy::SetTankParts(CGameObject* pLower, CGameObject* pUpper, CGameObject* pBarrel)
{
    m_pLowerBody = pLower;
    m_pUpperBody = pUpper;
    m_pBarrel = pBarrel;
}

bool CTankEnemy::CheckCollisionWith(CGameObject* pOther)
{
    bool Check = false;

    if (m_pUpperBody)
    {
        Check = Check || m_pUpperBody->CheckCollisionWith(pOther);
    }

    if (m_pLowerBody)
    {
        Check = Check || m_pLowerBody->CheckCollisionWith(pOther);
    }

    if (m_pBarrel)
    {
        Check = Check || m_pBarrel->CheckCollisionWith(pOther);
    }

    return Check;
}