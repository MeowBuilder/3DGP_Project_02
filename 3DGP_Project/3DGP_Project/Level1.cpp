#include "Level1.h"

CLevel1::CLevel1()
{
}

CLevel1::~CLevel1()
{
}

void CLevel1::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nShaders = 1;
	m_pShaders = new CLevel1Shader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	m_pPlayer = pAirplanePlayer;
	m_pPlayer->SetPosition(XMFLOAT3(0,0,0));
	m_pCamera = m_pPlayer->GetCamera();
}

void CLevel1::AnimateObjects(float fTimeElapsed) {
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);

		m_pShaders[i].AnimatePlayerOnRail(m_pPlayer, fTimeElapsed);

		if (m_pShaders[i].RailEnded())
		{
			SetNextSceneID(0);
			SetFinish(true);
		}
	}
}