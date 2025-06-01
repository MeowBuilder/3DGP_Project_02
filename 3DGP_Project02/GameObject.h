#pragma once

#include "Mesh.h"
#include "Camera.h"
#include <algorithm>

class CGameObject
{
public:
	CGameObject() {}
	virtual ~CGameObject();

public:
	bool						m_bActive = true;

	CMesh* m_pMesh = NULL;
	XMFLOAT4X4					m_xmf4x4World = Matrix4x4::Identity();

	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

	CGameObject* m_pObjectCollided = NULL;
	DWORD						m_dwColor = RGB(255, 0, 0);

	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;

	XMFLOAT3					m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float						m_fRotationSpeed = 0.0f;

public:
	BoundingOrientedBox GetOOBB() const { return m_xmOOBB; };

	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetMesh(CMesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	CMesh* GetMesh() { if (m_pMesh) return m_pMesh; else return nullptr; };

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetWorldMatrix(const XMFLOAT4X4& xmf4x4World);
	void SetRotationTransform(XMFLOAT4X4* pmxf4x4Transform);

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void UpdateBoundingBox();

	void Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh);

	virtual void OnUpdateTransform() {}
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	void GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection);
	virtual int PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);
};

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;

	CGameObject* m_pParentObject = nullptr;

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

public:
	static CMesh* m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};

class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	BoundingOrientedBox			m_xmOOBBPlayerMoveCheck = BoundingOrientedBox();
	XMFLOAT4					m_pxmf4WallPlanes[6];

	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingDelayTime = 0.3f;
	float						m_fLockingTime = 4.0f;
	CGameObject* m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void Reset();
};

class CTextCharacterObject : public CGameObject {
public:
	std::vector<CGameObject*> m_Cubes;

	static CCubeMesh* m_pSharedCubeMesh;

	CTextCharacterObject(wchar_t ch);
	virtual ~CTextCharacterObject();

	void SetColor(COLORREF color);

	void PrepareMesh();
	void ReleaseMesh();
	void BuildCharacterShape(wchar_t ch);

	virtual void Animate(float fElapsedTime) override;
	void Render(HDC hDCFrameBuffer, CCamera* pCamera, const XMFLOAT3& parentOffset) ;

	void SetRotationMatrix(const XMFLOAT4X4& xmf4x4Rotation) { m_xmf4x4Rotation = xmf4x4Rotation; }

	void SetRotatedPos(const XMFLOAT3 xmf3RotatedPos) { m_xmf3RotatedPos = xmf3RotatedPos; }
	XMFLOAT3 GetRotatedPos() { return m_xmf3RotatedPos; }

	virtual int PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance, const XMFLOAT3& parentOffset);
private:
	XMFLOAT4X4 m_xmf4x4Rotation = Matrix4x4::Identity();
	XMFLOAT3 m_xmf3RotatedPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class CTextObject : public CGameObject {
public:

	CTextObject(const std::wstring& text);
	virtual ~CTextObject();

	void SetTargetSceneID(int nSceneID) { m_nTargetSceneID = nSceneID; }
	int GetTargetSceneID() const { return m_nTargetSceneID; }

	void SetText(const std::wstring& text);
	void SetColor(COLORREF color);

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;

	virtual int PickObjectByRayIntersection(XMVECTOR& xmPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance);

	XMFLOAT3 getCenter() const {
		XMFLOAT3 center = { 0, 0, 0 };
		if (!m_Characters.empty())
		{
			size_t mid = m_Characters.size() / 2;
			center = m_Characters[mid]->GetPosition();
		}
		return center;
	}

private:
	float m_fRotationAngle = 0.0f;
	std::vector<CTextCharacterObject*> m_Characters;
	int m_nTargetSceneID = -1;
};

class CRailObject : public CGameObject
{
public:
	CRailObject();
	virtual ~CRailObject();
};