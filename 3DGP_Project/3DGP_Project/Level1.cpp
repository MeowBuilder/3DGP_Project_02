#include "Level1.h"

CLevel1::CLevel1()
{
}

CLevel1::~CLevel1()
{
}

void CLevel1::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nShaders = 1;
	m_pShaders = new CLevel1Shader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
}

bool CLevel1::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CLevel1::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CLevel1::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}