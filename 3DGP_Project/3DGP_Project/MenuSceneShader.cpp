#include "MenuSceneShader.h"

void CMenuSceneShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	CCubeMeshDiffused* pExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f);
	m_nObjects = 10;
	m_ppObjects = new CGameObject * [m_nObjects];

	float textZpos = 100.0f;

	CTextObject* pText1 = new CTextObject(L"TUTORIAL", pCubeMesh);
	pText1->SetPosition(0.0f,30.0f, textZpos);
	pText1->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pText1->SetRotationSpeed(10.0f);
	pText1->SetTargetSceneID(4);
	m_ppObjects[0] = pText1;

	CTextObject* pText2 = new CTextObject(L"LEVEL-1", pCubeMesh);
	pText2->SetPosition(-40.0f, 5.0f, textZpos);
	pText2->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pText2->SetRotationSpeed(10.0f);
	pText2->SetTargetSceneID(2);
	m_ppObjects[1] = pText2;

	CTextObject* pText3 = new CTextObject(L"LEVEL-2", pCubeMesh);
	pText3->SetPosition(40.0f, 5.0f, textZpos);
	pText3->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pText3->SetRotationSpeed(10.0f);
	pText3->SetTargetSceneID(3);
	m_ppObjects[2] = pText3;

	CTextObject* pText4 = new CTextObject(L"START", pCubeMesh);
	pText4->SetPosition(-40.0f, -20.0f, textZpos);
	pText4->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pText4->SetRotationSpeed(10.0f);
	pText4->SetTargetSceneID(2);
	m_ppObjects[3] = pText4;

	CTextObject* pText5 = new CTextObject(L"END", pCubeMesh);
	pText5->SetPosition(40.0f, -20.0f, textZpos);
	pText5->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pText5->SetRotationSpeed(10.0f);
	pText5->SetTargetSceneID(0);
	m_ppObjects[4] = pText5;

	CExplosiveObject* ExplosiveObject1 = NULL;
	ExplosiveObject1 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject1->Init(pText1);
	m_ppObjects[5] = ExplosiveObject1;
	pText1->SetExplosive(ExplosiveObject1);

	CExplosiveObject* ExplosiveObject2 = NULL;
	ExplosiveObject2 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject2->Init(pText2);
	m_ppObjects[6] = ExplosiveObject2;
	pText2->SetExplosive(ExplosiveObject2);

	CExplosiveObject* ExplosiveObject3 = NULL;
	ExplosiveObject3 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject3->Init(pText3);
	m_ppObjects[7] = ExplosiveObject3;
	pText3->SetExplosive(ExplosiveObject3);

	CExplosiveObject* ExplosiveObject4 = NULL;
	ExplosiveObject4 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject4->Init(pText4);
	m_ppObjects[8] = ExplosiveObject4;
	pText4->SetExplosive(ExplosiveObject4);

	CExplosiveObject* ExplosiveObject5 = NULL;
	ExplosiveObject5 = new CExplosiveObject(pCubeMesh);
	ExplosiveObject5->Init(pText5);
	m_ppObjects[9] = ExplosiveObject5;
	pText5->SetExplosive(ExplosiveObject5);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}