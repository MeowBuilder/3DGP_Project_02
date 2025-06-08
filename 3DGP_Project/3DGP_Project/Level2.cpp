#include "Level2.h"

CLevel2::CLevel2()
{
}

CLevel2::~CLevel2()
{
}

void CLevel2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nShaders = 1;
	m_pShaders = new CLevel2Shader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	CCubeMeshDiffused* pLowerBodyMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 8.0f,2.0f,12.0f);
	CCubeMeshDiffused* pUpperBodyMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f,2.0f,6.0f);
	CCubeMeshDiffused* pBarrelMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f,1.0f,6.0f);

	CGameObject* pLowerBody = new CGameObject();
	CGameObject* pUpperBody = new CGameObject();
	CGameObject* pBarrel = new CGameObject();

	pLowerBody = new CGameObject();
	pLowerBody->SetMesh((CMesh*)pLowerBodyMesh);

	pUpperBody = new CGameObject();
	pUpperBody->SetMesh((CMesh*)pUpperBodyMesh);
	pUpperBody->SetPosition(0, 2.0f, 0);

	pBarrel = new CGameObject();
	pBarrel->SetMesh((CMesh*)pBarrelMesh);
	pBarrel->SetPosition(0, 2.0f, 6.0f);

	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);

	CTankPlayer* pTankPlayer = new CTankPlayer();
	pTankPlayer->SetBulletMesh(pBulletMesh);
	pTankPlayer->Init(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	pTankPlayer->SetTankParts(pLowerBody, pUpperBody, pBarrel);
	m_pPlayer = pTankPlayer;
	m_pPlayer->SetPosition(XMFLOAT3(0, 0, 0));
	m_pCamera = m_pPlayer->GetCamera();
}

void CLevel2::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
		m_pShaders[i].CheckObjectCollision(m_pPlayer);
	}

	if (m_pPlayer) {
		m_pPlayer->Animate(fTimeElapsed);
		m_pPlayer->AutoFire(fTimeElapsed, m_pSelectedObject);
	}
}

bool CLevel2::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			SetNextSceneID(1);
			SetFinish(true);
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), fTimeElapsed);
			break;
		case 'a':
		case 'A':
			m_pPlayer->ToggleAutoFire();
			break;
		case 'w':
		case 'W':
			m_pShaders[0].SetTextOn();
			break;
		case 's':
		case 'S':
			static_cast<CTankPlayer*>(m_pPlayer)->ToggleShield();
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

void CLevel2::PickObjectPointedByCursor(int xClient, int yClient)
{
	if (!m_pCamera) return;
	XMFLOAT4X4 xmf4x4View = m_pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = m_pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = m_pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[i].PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}

	m_pSelectedObject = pNearestObject;
}