#include "StartSceneShader.h"

void CStartSceneShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	CCubeMeshDiffused* pExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f);
	m_nObjects = 4;
	m_ppObjects = new CGameObject * [m_nObjects];

	CTextObject* TextObject = NULL;
	TextObject = new CTextObject(L"3D게임 프로그래밍", pCubeMesh);
	TextObject->SetPosition(0.0f, 25.0f, 75.0f);
	TextObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	TextObject->SetRotationSpeed(10.0f);
	m_ppObjects[0] = TextObject;

	CTextObject* TextObject2 = NULL;
	TextObject2 = new CTextObject(L"김태순", pCubeMesh);
	TextObject2->SetPosition(XMFLOAT3(0.0f, -15.0f, 50.0f));
	TextObject2->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	TextObject2->SetRotationSpeed(10.0f);
	TextObject2->SetTargetSceneID(1);
	m_ppObjects[1] = TextObject2;

	CExplosiveObject* ExplosiveObject1 = NULL;
	ExplosiveObject1 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject1->Init(TextObject);
	m_ppObjects[2] = ExplosiveObject1;
	TextObject->SetExplosive(ExplosiveObject1);

	CExplosiveObject* ExplosiveObject2 = NULL;
	ExplosiveObject2 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject2->Init(TextObject2);
	m_ppObjects[3] = ExplosiveObject2;
	TextObject2->SetExplosive(ExplosiveObject2);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}