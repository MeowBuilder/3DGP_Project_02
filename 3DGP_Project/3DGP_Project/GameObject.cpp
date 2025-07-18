#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixIdentity());
	m_childExplosive = NULL;
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::Explosion() {
	if (m_childExplosive)
	{
		m_childExplosive->SetPosition(GetPosition());
		m_childExplosive->StartExplosion();
		m_bActive = false;
	}
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::Animate(float fTimeElapsed, const XMFLOAT4X4& parentWorld)
{
	XMMATRIX xmParent = XMLoadFloat4x4(&parentWorld);
	XMMATRIX xmLocal = XMLoadFloat4x4(&m_xmf4x4Transform);
	XMMATRIX xmWorld = XMMatrixMultiply(xmLocal, xmParent);
	XMStoreFloat4x4(&m_xmf4x4World, xmWorld);
}

void CGameObject::LookTo(XMFLOAT3& lookDirection, XMFLOAT3& upDirection)
{
	XMFLOAT3 zAxis = Vector3::Normalize(lookDirection);
	XMFLOAT3 xAxis = Vector3::Normalize(Vector3::CrossProduct(upDirection, zAxis));
	XMFLOAT3 yAxis = Vector3::CrossProduct(zAxis, xAxis);

	m_xmf4x4World._11 = xAxis.x; m_xmf4x4World._12 = xAxis.y; m_xmf4x4World._13 = xAxis.z;
	m_xmf4x4World._21 = yAxis.x; m_xmf4x4World._22 = yAxis.y; m_xmf4x4World._23 = yAxis.z;
	m_xmf4x4World._31 = zAxis.x; m_xmf4x4World._32 = zAxis.y; m_xmf4x4World._33 = zAxis.z;
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (IsVisible(pCamera))
	{
		UpdateShaderVariables(pd3dCommandList);
		if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances)
{
	OnPrepareRender();

	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	OnPrepareRender();
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{

}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetOffset(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;
}

void CGameObject::SetOffset(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) {
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Shift);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

bool CGameObject::CheckCollisionWith(CGameObject* pOther)
{
	if (!m_pMesh || !pOther->GetMesh()) return false;

	BoundingOrientedBox obbA = m_pMesh->GetBoundingBox();
	BoundingOrientedBox obbB = pOther->GetMesh()->GetBoundingBox();

	BoundingOrientedBox obbAWorld, obbBWorld;
	obbA.Transform(obbAWorld, XMLoadFloat4x4(&m_xmf4x4World));
	obbB.Transform(obbBWorld, XMLoadFloat4x4(&pOther->GetWorldMAT()));

	return obbAWorld.Intersects(obbBWorld);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& 
	xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}
int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}


CExplosiveObject::CExplosiveObject(CMesh* pMesh)
{
	for (int i = 0; i < EXPLOSION_DEBRIS; ++i)
	{
		auto* debris = new CGameObject();
		debris->SetMesh(pMesh);
		m_DebrisObjects.push_back(debris);

		XMStoreFloat3(&m_vDirection[i], RandomUnitVectorOnSphere());
	}
}

CExplosiveObject::~CExplosiveObject()
{
	for (auto obj : m_DebrisObjects) delete obj;
	m_DebrisObjects.clear();
}

void CExplosiveObject::Init(CGameObject* pFromObject)
{
	m_fElapsedTime = 0.0f;
	m_bBlowingUp = false;
}

void CExplosiveObject::StartExplosion()
{
	for (int i = 0; i < EXPLOSION_DEBRIS; ++i)
	{
		m_DebrisObjects[i]->SetPosition(GetPosition());
	}
	m_fElapsedTime = 0.0f;
	m_bBlowingUp = true;
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (!m_bBlowingUp) return;

	m_fElapsedTime += fElapsedTime;

	if (m_fElapsedTime > m_fDuration)
	{
		m_bBlowingUp = false;
		m_bBlowEnd = true;
		return;
	}

	for (int i = 0; i < EXPLOSION_DEBRIS; ++i)
	{
		XMFLOAT3 dir = m_vDirection[i];
		XMFLOAT3 pos = m_DebrisObjects[i]->GetPosition();
		pos.x += dir.x * m_fExplosionSpeed * fElapsedTime;
		pos.y += dir.y * m_fExplosionSpeed * fElapsedTime;
		pos.z += dir.z * m_fExplosionSpeed * fElapsedTime;
		m_DebrisObjects[i]->SetPosition(pos);

		m_DebrisObjects[i]->Rotate(&dir, m_fExplosionRotation * fElapsedTime);
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	if (!m_bBlowingUp) return;

	for (auto& obj : m_DebrisObjects)
		obj->Render(cmdList, pCamera);
}

CBulletObject::CBulletObject(float fRange)
{
	m_fRange = fRange;
	m_bActive = false;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(const XMFLOAT3& pos)
{
	m_xmf3FirePosition = pos;
	SetPosition(pos);
	m_fTraveledDistance = 0.0f;
}

void CBulletObject::SetMovingDirection(XMFLOAT3& dir)
{
	m_xmf3Direction = Vector3::Normalize(dir);
	LookTo(m_xmf3Direction,XMFLOAT3(0.0f,1.0f,0.0f));
}

void CBulletObject::Animate(float fElapsedTime)
{
	if (!m_bActive) return;

	if (m_pLockedObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMVECTOR xmvPosition = XMLoadFloat3(&xmf3Position);

		XMFLOAT3 xmf3LockedObjectPosition = m_pLockedObject->GetPosition();
		XMVECTOR xmvLockedObjectPosition = XMLoadFloat3(&xmf3LockedObjectPosition);
		XMVECTOR xmvToLockedObject = xmvLockedObjectPosition - xmvPosition;
		xmvToLockedObject = XMVector3Normalize(xmvToLockedObject);

		XMVECTOR xmvMovingDirection = XMLoadFloat3(&m_xmf3Direction);
		xmvMovingDirection = XMVector3Normalize(XMVectorLerp(xmvMovingDirection, xmvToLockedObject, 0.25f));
		XMStoreFloat3(&m_xmf3Direction, xmvMovingDirection);
		LookTo(m_xmf3Direction, XMFLOAT3(0.0f, 1.0f, 0.0f));
	}

	float fDistance = m_fSpeed * fElapsedTime;
	m_fTraveledDistance += fDistance;

	if (m_fTraveledDistance >= m_fRange)
	{
		m_bActive = false;
		return;
	}

	MoveForward(fDistance);
}

bool CBulletObject::CheckCollisionWith(CGameObject* pOther)
{
	if (!m_pMesh || !pOther->GetMesh()) return false;

	BoundingOrientedBox obbA = m_pMesh->GetBoundingBox();
	BoundingOrientedBox obbB = pOther->GetMesh()->GetBoundingBox();

	BoundingOrientedBox obbAWorld, obbBWorld;
	obbA.Transform(obbAWorld, XMLoadFloat4x4(&m_xmf4x4World));
	obbB.Transform(obbBWorld, XMLoadFloat4x4(&pOther->GetWorldMAT()));

	return obbAWorld.Intersects(obbBWorld);
}