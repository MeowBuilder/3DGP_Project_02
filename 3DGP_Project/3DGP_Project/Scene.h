#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);
	virtual	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);

	virtual	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual	void ReleaseObjects();

	virtual	bool ProcessInput(HWND hWnd, float fTimeElapsed);
	virtual	void AnimateObjects(float fTimeElapsed);
	virtual	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual	void ReleaseUploadBuffers();

	virtual	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual	ID3D12RootSignature *GetGraphicsRootSignature();
	
	virtual	void PickObjectPointedByCursor(int xClient, int yClient);

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

	CCamera* m_pCamera = NULL;

	CPlayer* m_pPlayer = NULL;

	CGameObject* m_pSelectedObject = NULL;

	POINT m_ptOldCursorPos;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};