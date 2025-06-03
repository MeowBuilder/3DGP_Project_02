#pragma once
#include "GameObject.h"

class CTextObjectCharacter : public CGameObject
{
private:
	std::vector<CGameObject*> m_Cubes;
public:
	CTextObjectCharacter(wchar_t ch, CMesh* pMesh);
	virtual ~CTextObjectCharacter();

	void SetOffset(const XMFLOAT3& offset);

	virtual void Animate(float fTimeElapsed, const XMFLOAT4X4& parentWM);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void BuildCharacterShape(wchar_t ch, CMesh* pMesh);

	virtual int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
};

class CTextObject : public CGameObject
{
private:
	std::vector<CTextObjectCharacter*> m_Characters;

	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	CTextObject(const std::wstring& text, CMesh* pMesh);
	virtual ~CTextObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void BuildCharacterShape(const std::wstring& text, CMesh* pMesh);

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
};
