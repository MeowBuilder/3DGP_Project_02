#pragma once
#include "Scene.h"
#include "Level2Shader.h"

class CLevel2 : public CScene
{
public:
	CLevel2();
	~CLevel2();

	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);
};

