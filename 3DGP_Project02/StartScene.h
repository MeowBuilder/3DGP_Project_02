#pragma once

#include "Scene.h"

class CStartScene : public CScene
{
public:
	CStartScene();
	~CStartScene();

private:

	CTextObject* m_pHitText = NULL;

	std::vector<CTextObject*> m_pTextObjects;

	std::vector<CExplosiveObject*> m_pExplosions;

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void ProcessInput(UCHAR* pKeyBuffer) { /* �Է� ���� */ }
	virtual void ProcessMouseInput(float dx, float dy, bool bRightButton) { /* ȸ�� ���� */ }

	virtual void UpdateCamera(float fElapsedTime) override;

	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

