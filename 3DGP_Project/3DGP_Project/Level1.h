#pragma once
#include "Scene.h"
#include "Level1Shader.h"	

class CLevel1 : public CScene
{
	CLevel1();
	~CLevel1();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual	bool ProcessInput(UCHAR* pKeysBuffer);

	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

