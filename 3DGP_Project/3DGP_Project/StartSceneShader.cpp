#include "StartSceneShader.h"

void CStartSceneShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	m_nObjects = 1;
	m_ppObjects = new CGameObject * [m_nObjects];

	//CRotatingObject* gameobj = NULL;
	//gameobj = new CRotatingObject();
	//gameobj->SetMesh(pCubeMesh);
	//gameobj->SetPosition(0, 0, 0);
	//gameobj->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	//gameobj->SetRotationSpeed(10.0f);
	//m_ppObjects[0] = gameobj;

	CTextObject* TextObjeect = NULL;
	TextObjeect = new CTextObject(L"3D게임 프로그래밍", pCubeMesh);
	TextObjeect->SetPosition(0, 0, 0);
	TextObjeect->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	TextObjeect->SetRotationSpeed(10.0f);
	m_ppObjects[0] = TextObjeect;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}