#pragma once

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�. 
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	//�÷��̾ ���� x-��(Right), y-��(Up), z-��(Look)���� �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����. 
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�. 
	XMFLOAT3 m_xmf3Velocity;

	//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	XMFLOAT3 m_xmf3Gravity;

	//xz-��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����. 
	float m_fMaxVelocityXZ;
	//y-�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����. 
	float m_fMaxVelocityY;

	//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����. 
	float m_fFriction;
	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� �������̴�. 
	LPVOID m_pPlayerUpdatedContext;
	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pCameraUpdatedContext;

	//�÷��̾ ���� ������ ī�޶��̴�. 
	CCamera *m_pCamera = NULL;

	bool m_bAutoFire = false;
public:
	CPlayer();
	virtual ~CPlayer();
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& fGravity) { m_xmf3Gravity = fGravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	void ToggleAutoFire() { m_bAutoFire = !m_bAutoFire; }

	virtual void AutoFire(float fElapsedTime, CGameObject* pTarget) {};

	void SetPosition(XMFLOAT3& xmf3Position) {
		Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false);
	}

	void SetOrientation(const XMFLOAT3& right, const XMFLOAT3& up, const XMFLOAT3& look);

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	CCamera* GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	//�÷��̾ �̵��ϴ� �Լ��̴�. 
	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	//�÷��̾ ȸ���ϴ� �Լ��̴�.
	virtual void Rotate(float x, float y, float z);

	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�. 
	void Update(float fTimeElapsed);

	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�. 
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	//ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ��̴�.
	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�.
	virtual void OnPrepareRender();

	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� �������Ѵ�. 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer() {};
	virtual ~CAirplanePlayer();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};

#define BULLETS 16

class CTankPlayer : public CAirplanePlayer
{
protected:
	CGameObject* m_pLowerBody = nullptr;   // �Ϻ�
	CGameObject* m_pUpperBody = nullptr;   // ��ž
	CGameObject* m_pBarrel = nullptr;   // ����

	CCubeMeshDiffused* m_pBulletMesh;
	CBulletObject* m_ppBullets[BULLETS];
	float m_fBulletRange = 1500.0f;

	float m_fAutoFireElapsed = 0.0f;
	float m_fAutoFireInterval = 1.0f;
	bool m_bShield = false;

public:
	CTankPlayer() {};
	virtual ~CTankPlayer();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void Rotate(float fPitch, float fYaw, float fRoll) override;

	virtual void Animate(float fElapsedTime) override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void SetBulletMesh(CCubeMeshDiffused* pMesh) { m_pBulletMesh = pMesh; }

	void SetTankParts(CGameObject* pLower, CGameObject* pUpper, CGameObject* pBarrel);
	void RotateTurret(float fAngle);
	void RotateLower(float fAngle);

	void FireBullet(CGameObject* pTarget);
	virtual void AutoFire(float fElapsedTime, CGameObject* pTarget);

	std::vector<CBulletObject*> GetBullets() {
		std::vector<CBulletObject*> bullets;
		for (int i = 0; i < BULLETS; ++i)
			bullets.push_back(m_ppBullets[i]);
		return bullets;
	}

	void ToggleShield() { m_bShield = !m_bShield; }
	bool GetShield() const { return m_bShield; }

	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
};