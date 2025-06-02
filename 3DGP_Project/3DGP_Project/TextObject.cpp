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

void CTextObjectCharacter::Animate(float fTimeElapsed, XMFLOAT4X4& parentWM)
{
	for (auto cube : m_Cubes)
		cube->Animate(fTimeElapsed);
}

void CTextObjectCharacter::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto cube : m_Cubes)
		cube->Render(pd3dCommandList,pCamera);
}

void CTextObjectCharacter::BuildCharacterShape(wchar_t ch, CMesh* pMesh)
{
	auto AddCube = [&](int x, int y)
		{
			CGameObject* pCube = new CGameObject();
			pCube->SetMesh(pMesh);
			pCube->SetPosition((float)x, (float)y, 0.0f);
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

CTextObject::CTextObject(const std::wstring& text, CMesh* pMesh)
{

}

CTextObject::~CTextObject()
{

}

void CTextObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	for (auto cube : m_Characters)
		cube->Animate(fTimeElapsed,m_xmf4x4World);
}

void CTextObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto character : m_Characters)
		character->Render(pd3dCommandList, pCamera);
}

void CTextObject::BuildCharacterShape(const std::wstring& text, CMesh* pMesh)
{
	float xOffset = 0.0f;
	const float spacing = 8.0f;

	for (wchar_t ch : text)
	{
		CTextObjectCharacter* pChar = new CTextObjectCharacter(ch, pMesh);
		pChar->SetPosition(XMFLOAT3(xOffset, 0.0f, 0.0f));
		xOffset += spacing;
		m_Characters.push_back(pChar);
	}
}