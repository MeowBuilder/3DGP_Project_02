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
        m_fRotationAngle = static_cast<float>((rand() % 180) - 90); // -90 ~ +90도
        m_fElapsedTime = 0.0f;
        m_fRandomMoveTime = 1.0f + ((float)(rand() % 200) / 100.0f);
    }

    // 회전
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

    // 이동
    MoveForward(m_iMoveDirection * 5.0f * fElapsedTime);

    // 하부
    if (m_pLowerBody)
    {
        m_pLowerBody->Animate(fElapsedTime, m_xmf4x4World);
    }

    if (m_pUpperBody)
    {
        m_pUpperBody->Animate(fElapsedTime, m_pLowerBody->GetWorldMAT());
    }

    // 포신
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

    // 하부
    if (m_pLowerBody)
    {
        m_pLowerBody->Render(pd3dCommandList, pCamera);
    }

    // 포신
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

    // 포탑 우선 검사
    if (m_pUpperBody)
    {
        if (m_pUpperBody->PickObjectByRayIntersection(pickPosition, viewMatrix, &fHitDistance))
        {
            fNearestHitDistance = fHitDistance;
            nIntersected = 1;
        }
    }

    // 하부
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

    // 포신
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

// 여기 만들어야 함 ㄹㅇㄹㅇ 오버라이드 해라
bool CGameObject::CheckCollisionWith(CGameObject* pOther)
{
    if (!m_pMesh || !pOther->m_pMesh) return false;

    BoundingOrientedBox obbA = m_pMesh->GetBoundingBox();
    BoundingOrientedBox obbB = pOther->m_pMesh->GetBoundingBox();

    BoundingOrientedBox obbAWorld, obbBWorld;
    obbA.Transform(obbAWorld, XMLoadFloat4x4(&m_xmf4x4World));
    obbB.Transform(obbBWorld, XMLoadFloat4x4(&pOther->m_xmf4x4World));

    return obbAWorld.Intersects(obbBWorld);
}