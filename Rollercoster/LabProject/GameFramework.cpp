//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework(){
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	//m_pd3dDeviceContext = NULL;
	//m_pd3dRenderTargetView = NULL;

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework(){
}

// 프로그래밍 싱행되면 호출되는 함수
// 프레임 워크 객체 초기화
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) {
	::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer(); 

	BuildObjects(); 
	
	return(true);
}

void CGameFramework::BuildFrameBuffer() {
    HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor){
    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight);
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer(){    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

// 마우스 메세지
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;

		case WM_MOUSEMOVE:
			break;

		default:
			break;
	}
}

// 키보드 메세지
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
		case WM_KEYDOWN:
			switch (wParam) {
				
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				
				default:
					m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
					break;
			}
			break;

		default:
			break;
	}
}

// 윈도우 메세지
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	
	switch (nMessageID) {
		
	case WM_ACTIVATE: 	{
			if (LOWORD(wParam) == WA_INACTIVE)	m_GameTimer.Stop();
			else	m_GameTimer.Start();
			break;
		}

		case WM_SIZE:
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;


		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
		return(0);
}

// 객체 생성
void CGameFramework::BuildObjects() {
	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(2.0f, 2.0f, 0.7f);
	m_pPlayer = new CAirplanePlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, -30.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -10.0f));

	m_pScene = new CPlayerScene();
	m_pScene->BuildObjects();

	m_pScene->m_pPlayer = m_pPlayer;
}

// 객체 삭제
void CGameFramework::ReleaseObjects() {
	if (m_pScene) 	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pPlayer) delete m_pPlayer;
}

// 종료될때
void CGameFramework::OnDestroy() {
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) 
		::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) 
		::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

// 외부입력 (마우스, 키보드)
void CGameFramework::ProcessInput() {
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	MoveTrain = false;

	if (GetKeyboardState(pKeyBuffer)) {
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		//if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		//if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		//if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		//if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		//if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd) 	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		// 나눈값  == 가중치
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 40.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 40.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	float distance = 0.f;
	
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)) {
		if (cxDelta || cyDelta) {
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) {
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				distance += 0.03;
			}
			else if (pKeyBuffer[VK_LBUTTON] & 0xF0) {
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
				distance += 0.03;
			}
		}
		// 키보드 입력으로 이동할때
		if (dwDirection)		distance = 0.05f;

		// player look vecter 
		XMFLOAT3 playerlook = m_pPlayer->GetLook();
		
		//위로 이동
		if (playerlook.y > 0)	distance -= 0.015;

		//아래로 이동
		else if (playerlook.y < 0)	distance += 0.015;


		MoveTrain = true;
	}

	m_pPlayer->Move(1,distance);

	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

// 실 게임 프레임
void CGameFramework::FrameAdvance() {    
    if (!m_bActive) return;

	m_GameTimer.Tick(0.0f);

	ProcessInput();

	if (!m_GameTimer.GetTimeCreateRail())
		m_pScene->CreateRail(m_pPlayer);
	
	m_pScene->UpdateFrontRail(m_pPlayer);
	
	ClearFrameBuffer(RGB(255, 255, 255));

	m_pScene->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera, MoveTrain);
	m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


