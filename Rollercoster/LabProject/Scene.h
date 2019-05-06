#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;
	CPlayer						*m_pTrain = NULL;

	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL;
	
	int m_rails= 0;
	int m_pointrail = 0;
	CGameObject **m_pprail = NULL;

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();
		
	virtual void Animate(float fElapsedTime);
	virtual void CreateRail(CPlayer* p);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CPlayerScene : public CScene
{
public:
	CPlayerScene();
	virtual ~CPlayerScene();

	virtual void BuildObjects();
};
