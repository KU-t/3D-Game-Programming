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

	// 다렉3D 그래픽 출력
	void FrameAdvance();		// 호출 함수 : ProcessInput, AnimateObject

	void SetActive(bool bActive) { m_bActive = bActive; }

private:
	// 프로그램의 인스턴스 핸들
	HINSTANCE				m_hInstance = NULL;
	// 윈도우 핸들
	HWND						m_hWnd = NULL;

    bool							m_bActive = true;          

	CGameTimer			m_GameTimer;

	HDC							m_hDCFrameBuffer = NULL;
    HBITMAP					m_hBitmapFrameBuffer = NULL;

	CPlayer						*m_pPlayer = NULL;
	//CPlayer						*m_pTrain = NULL;

	CAirplaneMesh			*m_pAirplaneMesh = NULL;
	CCubeMesh				*m_pCubeMesh = NULL;

	CScene						*m_pScene = NULL;

	// Direct3D 디바이스 인터페이스
	ID3D12Device *m_pd3dDevice;

	// SwapChain 인터페이스
	IDXGISwapChain *m_pDXGISwapChain;

	//Device Context 인터페이스
	//ID3D11DeviceContext *m_pd3dDeviceContext;

	// Render Target View 인터페이스
	//ID#D11RenderTargetView *m_pd3dRenderTargetView;

public:
	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	// 렌더링할 Mesh & Object - 생성, 소멸
	void BuildObjects();
	void ReleaseObjects();

	// 사용자 입력 처리 - 마우스, 키보드
	void ProcessInput();

	// 윈도우 입력 메시지 처리 - 마우스, 키보드
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 윈도우 메시지 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	POINT						m_ptOldCursorPos;

	_TCHAR					m_pszFrameRate[50];
};

