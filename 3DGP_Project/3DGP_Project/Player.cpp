#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer()
{
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::SetOrientation(const XMFLOAT3& right, const XMFLOAT3& up, const XMFLOAT3& look)
{
	m_xmf3Right = right;
	m_xmf3Up = up;
	m_xmf3Look = look;
}

void CPlayer::Move(ULONG nDirection, float fDistance, bool bVelocity)
{
	if (nDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (nDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (nDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);

		if (nDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (nDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);

		if (nDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (nDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
				XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look),
				XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	DWORD nCameraMode = m_pCamera->GetMode();

	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}

	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);

		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}
	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;
	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}
void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

void CAirplanePlayer::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {
	CMesh* pAirplaneMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f));
	SetMesh(pAirplaneMesh);

	m_pCamera = ChangeCamera(SPACESHIP_CAMERA, 0.0f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}

CAirplanePlayer::~CAirplanePlayer()
{
}

void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));

	Update(fTimeElapsed);
	return(m_pCamera);
}

void CTankPlayer::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

	for (int i = 0; i < BULLETS; ++i)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletRange);
		m_ppBullets[i]->SetActive(false);
		m_ppBullets[i]->SetMesh(m_pBulletMesh);
	}
}

CTankPlayer::~CTankPlayer()
{
	for (int i = 0; i < BULLETS; ++i)
		if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CTankPlayer::SetTankParts(CGameObject* pLower, CGameObject* pUpper, CGameObject* pBarrel)
{
	m_pLowerBody = pLower;
	m_pUpperBody = pUpper;
	m_pBarrel = pBarrel;
}

void CTankPlayer::Animate(float fElapsedTime)
{
	for (int i = 0; i < BULLETS; i++)
		if (m_ppBullets[i]->GetActive()) m_ppBullets[i]->Animate(fElapsedTime);
}

void CTankPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	if (m_pLowerBody) m_pLowerBody->Render(pd3dCommandList, pCamera);
	if (m_pUpperBody) m_pUpperBody->Render(pd3dCommandList, pCamera);
	if (m_pBarrel)    m_pBarrel->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < BULLETS; i++)
		if (m_ppBullets[i]->GetActive()) m_ppBullets[i]->Render(pd3dCommandList, pCamera);
}

void CTankPlayer::RotateTurret(float fAngle)
{
	if (m_pUpperBody)
		m_pUpperBody->Rotate(&m_xmf3Up, fAngle);
}

void CTankPlayer::RotateLower(float fAngle)
{
	if (m_pLowerBody)
		m_pLowerBody->Rotate(&m_xmf3Up, fAngle);
}

void CTankPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	CPlayer::Rotate(fPitch, fYaw, fRoll);

	if (m_pUpperBody)
	{
		RotateTurret(fYaw);
	}

	if (m_pBarrel)
	{
		XMFLOAT3 center = m_pUpperBody->GetPosition();
		XMFLOAT3 barrelPos = m_pBarrel->GetPosition();
		XMFLOAT3 delta = Vector3::Subtract(barrelPos, center);
		XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(fYaw));
		XMVECTOR vDelta = XMLoadFloat3(&delta);
		vDelta = XMVector3TransformCoord(vDelta, rotY);
		XMFLOAT3 rotatedDelta;
		XMStoreFloat3(&rotatedDelta, vDelta);
		XMFLOAT3 newPos = Vector3::Add(center, rotatedDelta);
		m_pBarrel->SetPosition(newPos);

		XMFLOAT3 look = Vector3::Normalize(Vector3::Subtract(newPos, center));
		m_pBarrel->LookTo(look,m_xmf3Up);
	}
}

void CTankPlayer::AutoFire(float fElapsedTime, CGameObject* pTarget)
{
	if (!m_bAutoFire) return;
	m_fAutoFireElapsed += fElapsedTime;
	if (m_fAutoFireElapsed >= m_fAutoFireInterval)
	{
		FireBullet(pTarget);
		m_fAutoFireElapsed = 0.0f;
	}
}

void CTankPlayer::FireBullet(CGameObject* pTarget)
{
	for (int i = 0; i < BULLETS; ++i)
	{
		if (!m_ppBullets[i]->GetActive())
		{
			XMFLOAT3 firePos = m_pBarrel->GetPosition();
			firePos = Vector3::Add(firePos, Vector3::ScalarProduct(m_pBarrel->GetLook(),6.0f,false));
			XMFLOAT3 direction = m_pBarrel ? m_pBarrel->GetLook() : GetLook();

			m_ppBullets[i]->SetFirePosition(firePos);
			m_ppBullets[i]->SetMovingDirection(direction);
			m_ppBullets[i]->SetActive(true);
			m_ppBullets[i]->m_pLockedObject = pTarget;

			break;
		}
	}
}

void CTankPlayer::Move(ULONG nDirection, float fDistance, bool bVelocity)
{
	SetPrePosition(GetPosition());

	if (nDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (nDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_pLowerBody->GetLook(), fDistance);
		if (nDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_pLowerBody->GetLook(), -fDistance);

		if (nDirection & DIR_RIGHT) RotateLower(fDistance);
		if (nDirection & DIR_LEFT) RotateLower(-fDistance);

		Move(xmf3Shift, bVelocity);
		if (m_pLowerBody)
		{
			m_pLowerBody->Move(xmf3Shift, bVelocity);
		}

		if (m_pUpperBody)
		{
			m_pUpperBody->Move(xmf3Shift, bVelocity);
		}

		if (m_pBarrel)
		{
			m_pBarrel->Move(xmf3Shift, bVelocity);
		}
	}
}

void CTankPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}

bool CTankPlayer::CheckCollisionWith(CGameObject* pOther)
{
	bool Check = false;

	if (m_pUpperBody)
	{
		Check = Check || pOther->CheckCollisionWith(m_pUpperBody);
	}

	if (m_pLowerBody)
	{
		Check = Check || pOther->CheckCollisionWith(m_pLowerBody);
	}

	if (m_pBarrel)
	{
		Check = Check || pOther->CheckCollisionWith(m_pBarrel);
	}

	return Check;
}

bool CTankPlayer::CheckCollisionWith(CTankEnemy* pEnemy) {
	bool Check = false;

	if (m_pUpperBody)
	{
		Check = Check || pEnemy->CheckCollisionWith(m_pUpperBody);
	}

	if (m_pLowerBody)
	{
		Check = Check || pEnemy->CheckCollisionWith(m_pLowerBody);
	}

	if (m_pBarrel)
	{
		Check = Check || pEnemy->CheckCollisionWith(m_pBarrel);
	}

	return Check;
}

void CTankPlayer::SetPosition(XMFLOAT3& xmf3Position) {
	Move(Vector3::Subtract(xmf3Position, m_xmf3Position), false);

	if (m_pLowerBody)
		m_pLowerBody->SetPosition(xmf3Position);

	XMFLOAT3 upperOffset = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 barrelOffset = XMFLOAT3(0.0f, 2.0f, 6.0f);

	XMMATRIX xmRotation = XMMatrixIdentity();
	xmRotation.r[0] = XMLoadFloat3(&m_xmf3Right);
	xmRotation.r[1] = XMLoadFloat3(&m_xmf3Up);
	xmRotation.r[2] = XMLoadFloat3(&m_xmf3Look);

	XMVECTOR vUpperOffset = XMVector3TransformCoord(XMLoadFloat3(&upperOffset), xmRotation);
	XMVECTOR vBarrelOffset = XMVector3TransformCoord(XMLoadFloat3(&barrelOffset), xmRotation);

	XMFLOAT3 upperPos, barrelPos;
	XMStoreFloat3(&upperPos, XMVectorAdd(XMLoadFloat3(&xmf3Position), vUpperOffset));
	XMStoreFloat3(&barrelPos, XMVectorAdd(XMLoadFloat3(&xmf3Position), vBarrelOffset));

	if (m_pUpperBody) m_pUpperBody->SetPosition(upperPos);
	if (m_pBarrel) m_pBarrel->SetPosition(barrelPos);
}