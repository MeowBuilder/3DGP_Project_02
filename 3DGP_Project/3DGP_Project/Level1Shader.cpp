#include "Level1Shader.h"
#include "Player.h"

void CLevel1Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 10.0f);

	BuildRailSegments();
	BuildRailObjects(pCubeMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CLevel1Shader::AnimateObjects(float fTimeElapsed) {
    for (int j = 0; j < m_nObjects; j++)
    {
        if (m_ppObjects[j]->GetActive())
            m_ppObjects[j]->Animate(fTimeElapsed);
    }
}

void CLevel1Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
    CShader::Render(pd3dCommandList, pCamera);
    for (int j = 0; j < m_nObjects; j++)
    {
        if (m_ppObjects[j])
        {
            if (m_ppObjects[j]->GetActive())
                m_ppObjects[j]->Render(pd3dCommandList, pCamera);
        }
    }

    for (auto& pRail : m_pRailObjects)
        pRail->Render(pd3dCommandList, pCamera);
}


void CLevel1Shader::BuildRailSegments() {
    std::vector<XMFLOAT3> m_ControlPoints;

    m_ControlPoints.push_back(XMFLOAT3(100.0f, 0.0f, 0.0f));
    m_ControlPoints.push_back(XMFLOAT3(0.0f, 0.0f, 100.0f));
    m_ControlPoints.push_back(XMFLOAT3(-100.0f, 0.0f, 0.0f));
    m_ControlPoints.push_back(XMFLOAT3(0.0f, 0.0f, -100.0f));
    m_ControlPoints.push_back(XMFLOAT3(0.0f, 100.0f, -100.0f));
    m_ControlPoints.push_back(XMFLOAT3(0.0f, 200.0f, 0.0f));
    m_ControlPoints.push_back(XMFLOAT3(0.0f, 100.0f, 100.0f));
    m_ControlPoints.push_back(XMFLOAT3(100.0f, 0.0f, 0.0f));

    std::vector<XMFLOAT3> tempPoints = m_ControlPoints;
    tempPoints.insert(tempPoints.begin(), m_ControlPoints[m_ControlPoints.size() - 2]);
    tempPoints.push_back(m_ControlPoints[1]);

    const int interpolateSteps = 10;

    for (size_t i = 0; i < tempPoints.size() - 3; ++i)
    {
        for (int step = 0; step < interpolateSteps; ++step)
        {
            float t = (float)step / (float)interpolateSteps;
            XMFLOAT3 pos = Vector3::CatmullRom(
                tempPoints[i],
                tempPoints[i + 1],
                tempPoints[i + 2],
                tempPoints[i + 3],
                t
            );

            XMFLOAT3 nextPos = Vector3::CatmullRom(
                tempPoints[i],
                tempPoints[i + 1],
                tempPoints[i + 2],
                tempPoints[i + 3],
                t + (1.0f / interpolateSteps)
            );

            XMFLOAT3 tangent = Vector3::Normalize(Vector3::Subtract(nextPos, pos));
            m_RailSegments.push_back({ pos, tangent, 0.0f });
        }
    }
}

void CLevel1Shader::BuildRailObjects(CMesh* pMesh) {
    for (const auto& segment : m_RailSegments)
    {
        CRailObject* pRail = new CRailObject(pMesh);
        pRail->SetPosition(segment.position.x, segment.position.y, segment.position.z);

        XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
        XMFLOAT3 tangentCopy = segment.tangent;
        pRail->LookTo(tangentCopy, up);

        m_pRailObjects.push_back(pRail);
    }
}

void CLevel1Shader::AnimatePlayerOnRail(CPlayer* pPlayer,float fTimeElapsed) {
    m_fRailProgress += speed * fTimeElapsed;

    if (m_fRailProgress >= 1.0f)
    {
        m_fRailProgress = 0.0f;
        m_nCurrentRailIndex++;

        if (m_nCurrentRailIndex >= m_RailSegments.size())
        {
            m_bRailEnded = true;
            m_nCurrentRailIndex = 0;
            return;
        }
    }

    const RailSegment& seg0 = m_RailSegments[m_nCurrentRailIndex];
    const RailSegment& seg1 = m_RailSegments[(m_nCurrentRailIndex + 1) % m_RailSegments.size()];

    XMFLOAT3 pos = Vector3::Lerp(seg0.position, seg1.position, m_fRailProgress);
    pPlayer->SetPosition(pos);

    pPlayer->Animate(fTimeElapsed);
}