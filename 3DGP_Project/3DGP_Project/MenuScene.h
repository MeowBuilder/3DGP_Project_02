#pragma once
#include "Scene.h"
#include "MenuSceneShader.h"

class CMenuScene : public CScene
{
public:
	CMenuScene();
	~CMenuScene();

	virtual bool ProcessInput(HWND hWnd, float fTimeElapsed) { return false; }
	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);
};

