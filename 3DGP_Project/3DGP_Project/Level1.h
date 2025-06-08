#pragma once
#include "Scene.h"
#include "Level1Shader.h"

class CLevel1 : public CScene
{
public:
	CLevel1();
	~CLevel1();

	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);

	virtual void AnimateObjects(float fTimeElapsed);
};

