#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;
	CPlayer						*m_pTrain = NULL;

	int m_maxrails;
	int m_pointrail;
	CGameObject **m_pprail = NULL;

	int m_maxfrontrails;
	CGameObject **m_ppfrontrail = NULL;

	CGameObject	*m_pTerrainObject = NULL;

	int					MaxCreateRailTime;
	int					CreateRailTime;

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();
		
	virtual void Animate(float fElapsedTime);
	virtual void CreateRail(CPlayer* p);
	virtual void UpdateBackRail();
	virtual void UpdateFrontRail(CPlayer * player);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera, const bool& movetrain);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CPlayerScene : public CScene{
public:
	CPlayerScene();
	virtual ~CPlayerScene();

	virtual void BuildObjects();
};
