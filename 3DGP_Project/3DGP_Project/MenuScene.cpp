#include "MenuScene.h"

CMenuScene::CMenuScene()
{
}

CMenuScene::~CMenuScene()
{
}

void CMenuScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nShaders = 1;
	m_pShaders = new CMenuSceneShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer();
	pAirplanePlayer->Init(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	m_pPlayer = pAirplanePlayer;
	m_pPlayer->SetPosition(XMFLOAT3(0, 0, 0));
	m_pCamera = m_pPlayer->GetCamera();
}

bool CMenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			SetNextSceneID(0);
			SetFinish(true);
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), fTimeElapsed);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return(false);
}