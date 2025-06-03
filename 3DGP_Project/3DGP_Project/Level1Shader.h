#pragma once
#include "Shader.h"

struct RailSegment
{
	XMFLOAT3 position;
	XMFLOAT3 tangent;
	float rotation = 0.0f;
};

class CLevel1Shader : public CObjectsShader
{
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void AnimatePlayerOnRail(CPlayer* pPlayer, float fElapsedTime);

	virtual XMFLOAT3 GetStartPosition() {
		return m_RailSegments[0].position;
	}
private:
	void BuildRailSegments();
	void BuildRailObjects(CMesh* pMesh);
private:
	std::vector<RailSegment> m_RailSegments;
	std::vector<CRailObject*> m_pRailObjects;

	bool m_bRailEnded = false;

	int m_nCurrentRailIndex = 0;
	float m_fRailProgress = 0.0f;
	float speed = 5.0f;
	float fRotateLerpSpeed = 5.0f;
};

