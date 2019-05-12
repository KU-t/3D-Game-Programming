#pragma once

//-----------------------------------------------------------------------------
// File: GameFramework.h
//-----------------------------------------------------------------------------

#include "Timer.h"
#include "Scene.h"
#include "Player.h"

class CGameFramework{
public:
	CGameFramework(void);
	~CGameFramework(void);

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	// �ٷ�3D �׷��� ���
	void FrameAdvance();		// ȣ�� �Լ� : ProcessInput, AnimateObject

	void SetActive(bool bActive) { m_bActive = bActive; }

private:
	// ���α׷��� �ν��Ͻ� �ڵ�
	HINSTANCE				m_hInstance = NULL;
	// ������ �ڵ�
	HWND						m_hWnd = NULL;

    bool							m_bActive = true;          
	bool							MoveTrain = false;

	CGameTimer			m_GameTimer;

	HDC							m_hDCFrameBuffer = NULL;
    HBITMAP					m_hBitmapFrameBuffer = NULL;

	CPlayer						*m_pPlayer = NULL;

	CAirplaneMesh			*m_pAirplaneMesh = NULL;
	
	CScene						*m_pScene = NULL;

	// Direct3D ����̽� �������̽�
	ID3D12Device *m_pd3dDevice;

	// SwapChain �������̽�
	IDXGISwapChain *m_pDXGISwapChain;

public:
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	// �������� Mesh & Object - ����, �Ҹ�
	void BuildObjects();
	void ReleaseObjects();

	// ����� �Է� ó�� - ���콺, Ű����
	void ProcessInput();

	// ������ �Է� �޽��� ó�� - ���콺, Ű����
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// ������ �޽��� ó��
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	POINT						m_ptOldCursorPos;

	_TCHAR					m_pszFrameRate[50];
};

