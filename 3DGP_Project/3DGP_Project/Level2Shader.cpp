#include "Level2Shader.h"

void CLevel2Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
    CCubeMeshDiffused* pExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.75f, 0.75f, 0.75f);
    m_nObjects = 1;
    m_ppObjects = new CGameObject * [m_nObjects];

    int curObject = 0;

    CTextObject* TextObject = NULL;
    TextObject = new CTextObject(L"YOU WIN!", pCubeMesh);
    TextObject->SetPosition(0.0f, 10.0f, 0.0f);
    TextObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
    TextObject->SetRotationSpeed(10.0f);
    TextObject->SetActive(false);
    m_ppObjects[curObject++] = TextObject;

    const float fBottomHeight = 2.0f;
    const float fUpperHeight = 2.0f;
    const float fUpperWidth = 3.0f;

    CExplosiveObject* ExplosiveObject = NULL;

    for (int i = 0; i < 10; ++i)
    {
        auto* pLowerMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 8.0f, fBottomHeight, 12.0f);
        auto* pUpperMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, fUpperHeight, 6.0f);
        auto* pBarrelMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 6.0f);

        CGameObject* pLowerBody = new CGameObject();
        pLowerBody->SetMesh(pLowerMesh);

        CGameObject* pUpperBody = new CGameObject();
        pUpperBody->SetMesh(pUpperMesh);
        pUpperBody->SetOffset(0, fBottomHeight, 0);

        CGameObject* pBarrel = new CGameObject();
        pBarrel->SetMesh(pBarrelMesh);
        pBarrel->SetOffset(0, 0, fUpperWidth);

        CTankEnemy* pEnemy = new CTankEnemy();
        pEnemy->SetTankParts(pLowerBody, pUpperBody, pBarrel);

        float x = (rand() % 501) - 250;
        float z = (rand() % 501) - 250;
        pEnemy->SetPosition(XMFLOAT3(x, 0.0f, z));

        float yaw = static_cast<float>((rand() % 360) - 180);
        pEnemy->Rotate(0.0f, yaw, 0.0f);

        ExplosiveObject = new CExplosiveObject(pExplosionMesh);
        ExplosiveObject->Init(pEnemy);
        m_pExplosive.push_back(ExplosiveObject);
        pEnemy->SetExplosive(ExplosiveObject);

        m_pEnemies.push_back(pEnemy);
    }

    const int nObstacles = 20;
    const float obstacleSize = 10.0f;
    const float halfSize = obstacleSize * 0.5f;

    CCubeMeshDiffused* pObstacleMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, obstacleSize, obstacleSize, obstacleSize);

    for (int i = 0; i < nObstacles; )
    {
        CGameObject* pObstacle = new CGameObject();
        pObstacle->SetMesh(pObstacleMesh);

        bool bValid = false;

        while (!bValid)
        {
            float x = static_cast<float>((rand() % 501) - 250);
            float z = static_cast<float>((rand() % 501) - 250);

            if (x >= -5.0f && x <= 5.0f && z >= -5.0f && z <= 5.0f)
                continue;

            XMFLOAT3 pos = XMFLOAT3(x, halfSize, z);
            pObstacle->SetPosition(pos);

            bValid = true;

            for (auto& pEnemy : m_pEnemies)
            {
                if (pObstacle->CheckCollisionWith(pEnemy))
                {
                    bValid = false;
                    break;
                }
            }
        }

        m_pObstacles.push_back(pObstacle);
        ++i;
    }

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CLevel2Shader::AnimateObjects(float fTimeElapsed) {
    for (int j = 0; j < m_nObjects; j++)
    {
        if (m_ppObjects[j]->GetActive())
            m_ppObjects[j]->Animate(fTimeElapsed);
    }

    for (auto& pEnemy : m_pEnemies) {
        pEnemy->Animate(fTimeElapsed);
    }

    for (auto& pExplosive : m_pExplosive)
        pExplosive->Animate(fTimeElapsed);

    for (auto& pObstacles : m_pObstacles)
        pObstacles->Animate(fTimeElapsed);
}

void CLevel2Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
    CShader::Render(pd3dCommandList, pCamera);
    for (int j = 0; j < m_nObjects; j++)
    {
        if (m_ppObjects[j])
        {
            if (m_ppObjects[j]->GetActive())
                m_ppObjects[j]->Render(pd3dCommandList, pCamera);
        }
    }

    for (auto& pEnemy : m_pEnemies)
        pEnemy->Render(pd3dCommandList, pCamera);

    for (auto& pExplosive : m_pExplosive)
        pExplosive->Render(pd3dCommandList, pCamera);

    for (auto& pObstacles : m_pObstacles)
        pObstacles->Render(pd3dCommandList, pCamera);
}

CGameObject* CLevel2Shader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
    XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
    int nIntersected = 0;
    *pfNearHitDistance = FLT_MAX;
    float fHitDistance = FLT_MAX;
    CGameObject* pSelectedObject = NULL;

    for (auto& pEnemy : m_pEnemies) {
        if (pEnemy->GetActive()) {
            nIntersected = pEnemy->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
            if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
            {
                *pfNearHitDistance = fHitDistance;
                pSelectedObject = pEnemy;
            }
        }
    }


    return(pSelectedObject);
}

bool CLevel2Shader::CheckClear() {
    bool Clear = true;
    
    for (auto& pEnemy : m_pEnemies)
    {
        if (pEnemy->GetActive())
        {
            Clear = false;
            return Clear;
        }
    }

    return Clear;
}

void CLevel2Shader::SetTextOn() {
    m_ppObjects[0]->SetActive(true);
}

void CLevel2Shader::ExplodeEnemy(CGameObject* pEnemy) {
    pEnemy->Explosion();
}

void CLevel2Shader::ExplodePlayer(CPlayer* pPlayer) {
    pPlayer->Explosion();
    pPlayer->SetPosition(XMFLOAT3(0, 0, 0));
}

void CLevel2Shader::CheckObjectCollision(CPlayer* pPlayer)
{
    if (!pPlayer) return;

    auto bullets = static_cast<CTankPlayer*>(pPlayer)->GetBullets();
    for (auto& pBullet : bullets)
    {
        if (!pBullet->GetActive()) continue;

        for (auto& pEnemy : m_pEnemies)
        {
            if (pEnemy->GetActive() && pEnemy->CheckCollisionWith(pBullet))
            {
                pBullet->SetActive(false);
                pEnemy->Explosion();
                if (CheckClear())
                {
                    SetTextOn();
                }
            }
        }
    }

    for (auto& pEnemy : m_pEnemies)
    {
        if (!pEnemy->GetActive()) continue;

        if (static_cast<CTankPlayer*>(pPlayer)->CheckCollisionWith(pEnemy))
        {
            if (static_cast<CTankPlayer*>(pPlayer)->GetShield()) {
                ExplodeEnemy(pEnemy);
            }
            else {
                ExplodePlayer(pPlayer);
            }
        }
    }

    for (auto& pObstacle : m_pObstacles)
    {
        if (!pObstacle->GetActive()) continue;

        if (pPlayer->CheckCollisionWith(pObstacle))
        {
            pPlayer->SetPosition(static_cast<CTankPlayer*>(pPlayer)->GetPrePosition());
        }
    }

    for (auto& pEnemy : m_pEnemies)
    {
        if (!pEnemy->GetActive()) continue;

        for (auto& pObstacle : m_pObstacles)
        {
            if (!pObstacle->GetActive()) continue;

            if (pEnemy->CheckCollisionWith(pObstacle))
            {
                pEnemy->SetPosition(static_cast<CTankEnemy*>(pEnemy)->GetPrePosition());
            }
        }
    }

    for (auto& pObstacle : m_pObstacles)
    {
        if (!pObstacle->GetActive()) continue;

        for (auto& pBullet : bullets)
        {
            if (pBullet->GetActive() && pBullet->CheckCollisionWith(pObstacle))
            {
                pBullet->SetActive(false);
            }
        }
    }
}
