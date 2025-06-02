#pragma once
#include "Scene.h"
#include "StartSceneShader.h"

class CStartScene : public CScene
{
public:
	CStartScene();
	~CStartScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual	void ReleaseObjects();

	virtual	bool ProcessInput(UCHAR* pKeysBuffer);
	virtual	void AnimateObjects(float fTimeElapsed);
	virtual	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual	void ReleaseUploadBuffers();

	virtual	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual	ID3D12RootSignature* GetGraphicsRootSignature();

	virtual	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
protected:
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
};

