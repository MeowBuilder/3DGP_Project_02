#pragma once
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual	void ReleaseObjects();

	virtual	bool ProcessInput(UCHAR* pKeysBuffer);
	virtual	void AnimateObjects(float fTimeElapsed);
	virtual	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual	void ReleaseUploadBuffers();

	virtual	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual	ID3D12RootSignature *GetGraphicsRootSignature();

	virtual	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual bool IsFinished() const { return m_bSceneFinished; }
	virtual void SetFinish(bool bFinish) { m_bSceneFinished = bFinish; }
	virtual int GetNextSceneID() const { return m_nNextSceneID; }
	virtual void SetNextSceneID(int SceneID) { m_nNextSceneID = SceneID; }
protected:
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	int m_nNextSceneID = -1;
	bool m_bSceneFinished = false;

	CGameObject* m_EndObject = NULL;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};