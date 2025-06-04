#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;
class CExplosiveObject;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
protected:
	bool m_bActive = true;

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Transform; // ���� ��ȯ
	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;

	int m_nTargetSceneID = -1; // ��ŷ �Ǿ��� �� �Ѱ��� �� ID
	CExplosiveObject* m_childExplosive;

	XMFLOAT3 m_xmf3PrePos{ 0,0,0 };
public:
	void SetActive(bool bActive) { m_bActive = bActive; }
	bool GetActive() { return m_bActive; }

	void SetTargetSceneID(int nSceneID) { m_nTargetSceneID = nSceneID; } // �� ID ����
	int GetTargetSceneID() const { return m_nTargetSceneID; } // �� ID ��ȯ

	void SetExplosive(CExplosiveObject* child) { m_childExplosive = child; }
	CExplosiveObject* GetExplosive() { return m_childExplosive; }
	void Explosion();

	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, const XMFLOAT4X4& parentWorld);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);

	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPrePosition(XMFLOAT3 xmf3Pre) { m_xmf3PrePos = xmf3Pre; }
	XMFLOAT3 GetPrePosition() { return m_xmf3PrePos; }

	XMFLOAT4X4 GetWorldMAT() { return m_xmf4x4World; }
	void SetWorldMAT(XMFLOAT4X4 WorldMAT) { m_xmf4x4World = WorldMAT; }
	void LookTo(XMFLOAT3& lookDirection, XMFLOAT3& upDirection);

	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetOffset(float x, float y, float z);
	void SetOffset(XMFLOAT3 xmf3Position);

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	bool IsVisible(CCamera* pCamera = NULL);

	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);

	virtual int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);

	virtual bool CheckCollisionWith(CGameObject*);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis =
			xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};

#define EXPLOSION_DEBRIS 255

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject(CMesh* pMesh);
	virtual ~CExplosiveObject();

	void Init(CGameObject* pFromObject);

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void StartExplosion();
	bool IsFinished() const { return m_bBlowEnd; }
private:
	std::vector<CGameObject*> m_DebrisObjects;

	bool m_bBlowingUp = false;
	bool m_bBlowEnd = false;
	float m_fElapsedTime = 0.0f;
	float m_fDuration = 2.0f;

	float m_fExplosionSpeed = 10.0f;
	float m_fExplosionRotation = 720.0f;

	XMFLOAT3 m_vDirection[EXPLOSION_DEBRIS];

	static CMesh* m_pExplosionMesh;
};

class CRailObject : public CGameObject
{
public:
	CRailObject(CMesh* pMesh)
	{
		SetMesh(pMesh);
	}

	virtual ~CRailObject() {}
};

class CBulletObject : public CGameObject
{
protected:
	XMFLOAT3 m_xmf3FirePosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Direction = { 0.0f, 0.0f, 1.0f };

	float m_fRange = 1000.0f;
	float m_fTraveledDistance = 0.0f;
	float m_fSpeed = 120.0f;

public:
	CGameObject* m_pLockedObject = nullptr;

public:
	CBulletObject(float fRange = 1000.0f);
	virtual ~CBulletObject();

	void SetFirePosition(const XMFLOAT3& pos);
	void SetMovingDirection(XMFLOAT3& dir);
	void SetMovingSpeed(float fSpeed) { m_fSpeed = fSpeed; }

	virtual void Animate(float fElapsedTime) override;
};