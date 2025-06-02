#pragma once
#include "GameObject.h"
class CTextObjectCharacter : public CGameObject
{
private:
	std::vector<CGameObject*> m_Cubes;
public:
	CTextObjectCharacter(wchar_t ch, CMesh* pMesh);
	virtual ~CTextObjectCharacter();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4& parentWM);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void BuildCharacterShape(wchar_t ch, CMesh* pMesh);
};

class CTextObject : public CGameObject
{
private:
	std::vector<CTextObjectCharacter*> m_Characters;
	int m_nTargetSceneID = -1;

	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	CTextObject(const std::wstring& text, CMesh* pMesh);
	virtual ~CTextObject();

	void SetTargetSceneID(int nSceneID) { m_nTargetSceneID = nSceneID; }
	int GetTargetSceneID() const { return m_nTargetSceneID; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void BuildCharacterShape(const std::wstring& text, CMesh* pMesh);

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis)
	{
		m_xmf3RotationAxis = xmf3RotationAxis;
	}

};
