#pragma once
#include "Shader.h"
#include "TextObject.h"

class CStartSceneShader : public CObjectsShader
{
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

