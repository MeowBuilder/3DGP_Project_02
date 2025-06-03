#include "SceneManager.h"

CSceneManager::~CSceneManager()
{
    ReleaseCurrentScene();
}

void CSceneManager::SetCurrentScene(CScene* pScene, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    ReleaseCurrentScene();
    m_pCurrentScene = pScene;
    if (m_pCurrentScene) m_pCurrentScene->BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneManager::ChangeScene(int nSceneID, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    if (nSceneID == -1) return;

    switch (nSceneID)
    {
    case 0:
        SetCurrentScene(new CStartScene(), pd3dDevice, pd3dCommandList);
        break;
    case 1:
        SetCurrentScene(new CMenuScene(), pd3dDevice, pd3dCommandList);
        break;
    case 2:
        //SetCurrentScene(new CLevel1(), pd3dDevice, pd3dCommandList);
        break;
    case 3:
        //SetCurrentScene(new CLevel2(), pd3dDevice, pd3dCommandList);
        break;
    case 4:
        SetCurrentScene(new CScene(), pd3dDevice, pd3dCommandList);
        break;
    default:
        break;
    }
}

void CSceneManager::AnimateObjects(float fElapsedTime, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    if (m_pCurrentScene) {
        m_pCurrentScene->AnimateObjects(fElapsedTime);
    }
}

void CSceneManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
    if (m_pCurrentScene) m_pCurrentScene->Render(pd3dCommandList,pCamera);
}

void CSceneManager::ReleaseCurrentScene()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->ReleaseObjects();
        delete m_pCurrentScene;
        m_pCurrentScene = nullptr;
    }
}

void CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    if (m_pCurrentScene)
        m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    if (m_pCurrentScene)
        m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

CGameObject* CSceneManager::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) {
    return m_pCurrentScene->PickObjectPointedByCursor(xClient, yClient, pCamera);
}

ID3D12RootSignature* CSceneManager::GetGraphicsRootSignature() {
    return m_pCurrentScene->GetGraphicsRootSignature();
}

void CSceneManager::ReleaseUploadBuffers() {
    m_pCurrentScene->ReleaseUploadBuffers();
}

bool CSceneManager::IsFinished() {
    return m_pCurrentScene->IsFinished();
}