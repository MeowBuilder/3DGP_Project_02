#pragma once
#include "Scene.h"
#include "StartSceneShader.h"

class CStartScene : public CScene
{
public:
	CStartScene();
	~CStartScene();

	virtual	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};