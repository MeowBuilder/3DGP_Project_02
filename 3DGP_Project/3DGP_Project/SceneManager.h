#pragma once
#include "Scene.h"
#include "StartScene.h"
#include "MenuScene.h"
#include "Level1.h"
#include "Level2.h"

class CSceneManager
{
private:
    CScene* m_pCurrentScene;
public:
    CSceneManager() = default;
    ~CSceneManager();

    void SetCurrentScene(CScene* pScene, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    CScene* GetCurrentScene() const { return m_pCurrentScene; }

    void ChangeScene(int nSceneID, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

    void AnimateObjects(float fElapsedTime, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseCurrentScene();

    void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);
    void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, float fTimeElapsed);
    void ProcessInput(HWND hWnd, float fTimeElapsed);

    void ReleaseUploadBuffers();

    bool IsFinished();
};