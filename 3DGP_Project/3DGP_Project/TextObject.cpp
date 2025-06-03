#include "TextObject.h"
#include "TextGlyphTable.h"

CTextObjectCharacter::CTextObjectCharacter(wchar_t ch, CMesh* pMesh)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	BuildCharacterShape(ch, pMesh);
}

CTextObjectCharacter::~CTextObjectCharacter()
{
	for (auto cube : m_Cubes)
	{
		delete cube;
	}
	m_Cubes.clear();
}

void CTextObjectCharacter::SetOffset(const XMFLOAT3& offset)
{
	XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixTranslation(offset.x, offset.y, offset.z));
}

void CTextObjectCharacter::Animate(float fTimeElapsed, const XMFLOAT4X4& parentWorld)
{
	XMMATRIX xmParent = XMLoadFloat4x4(&parentWorld);
	XMMATRIX xmLocal = XMLoadFloat4x4(&m_xmf4x4Transform);
	XMMATRIX xmWorld = XMMatrixMultiply(xmLocal, xmParent);
	XMStoreFloat4x4(&m_xmf4x4World, xmWorld);

	for (auto cube : m_Cubes)
		cube->Animate(fTimeElapsed, m_xmf4x4World);
}

void CTextObjectCharacter::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto cube : m_Cubes)
	{
		cube->Render(pd3dCommandList, pCamera);
	}
}

void CTextObjectCharacter::BuildCharacterShape(wchar_t ch, CMesh* pMesh)
{
	auto AddCube = [&](int x, int y)
		{
			CGameObject* pCube = new CGameObject();
			pCube->SetMesh(pMesh);
			pCube->SetOffset((float)x, (float)y, 0.0f);
			m_Cubes.push_back(pCube);
		};

	const wchar_t** shape = GetGlyph(ch);
	if (!shape) return;

	for (int y = 0; y < GLYPH_HEIGHT; ++y)
	{
		const wchar_t* line = shape[GLYPH_HEIGHT - 1 - y];
		for (int x = 0; x < GLYPH_WIDTH; ++x)
		{
			if (line[x] == L'#')
			{
				AddCube(x, y);
			}
		}
	}
}

int CTextObjectCharacter::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance) {
	for (auto cube : m_Cubes)
	{
		if (cube->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance))
			return 1;  // 하나라도 맞으면 바로 성공
	}
	return 0;
}

CTextObject::CTextObject(const std::wstring& text, CMesh* pMesh)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	BuildCharacterShape(text, pMesh);
}

CTextObject::~CTextObject()
{

}

void CTextObject::Animate(float fTimeElapsed)
{
	XMMATRIX xmRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RotationAxis), XMConvertToRadians(m_fRotationSpeed * fTimeElapsed));
	XMMATRIX xmWorld = XMLoadFloat4x4(&m_xmf4x4World);
	xmWorld = XMMatrixMultiply(xmRotate, xmWorld);
	XMStoreFloat4x4(&m_xmf4x4World, xmWorld);

	for (auto character : m_Characters)
		character->Animate(fTimeElapsed, m_xmf4x4World);
}

void CTextObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto character : m_Characters)
	{
		character->Render(pd3dCommandList, pCamera);
	}
}

void CTextObject::BuildCharacterShape(const std::wstring& text, CMesh* pMesh)
{
	float spacing = 8.0f;
	float totalWidth = spacing * static_cast<float>(text.length());
	float startX = -totalWidth / 2.0f;

	float xOffset = startX;
	for (wchar_t ch : text)
	{
		auto* pChar = new CTextObjectCharacter(ch, pMesh);
		pChar->SetOffset(XMFLOAT3(xOffset, 0.0f, 0.0f));
		xOffset += spacing;
		m_Characters.push_back(pChar);
	}
}

int CTextObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance) {
	for (auto character : m_Characters)
	{
		if (character->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance))
			return 1;
	}
	return 0;
}