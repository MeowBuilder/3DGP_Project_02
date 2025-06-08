#pragma once
#include "Shader.h"
#include "TankEnemy.h"
#include "TextObject.h"
#include "Player.h"

class CLevel2Shader : public CObjectsShader
{
private:
	std::vector<CTankEnemy*> m_pEnemies;
	std::vector<CGameObject*> m_pObstacles;
	std::vector<CExplosiveObject*> m_pExplosive;
public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void SetTextOn();

	virtual void CheckObjectCollision(CPlayer* pPlayer);

	CGameObject* PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance);

	void ExplodeEnemy(CGameObject* pEnemy);
	void ExplodePlayer(CPlayer* pPlayer);
	bool CheckClear();
};