// LABPROJECT11.cpp : 응용 프로그램에 대한 진입점을 정의합니다.

#include "stdafx.h"
#include "LABPROJECT11.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

// 전역 변수

// 현재 인스턴스입니다.
HINSTANCE ghAppInstance;
// 제목 표시줄 텍스트입니다.
WCHAR szTitle[MAX_LOADSTRING];
// 기본 창 클래스 이름입니다.
WCHAR szWindowClass[MAX_LOADSTRING];

GameFramework gGameFrameWork;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow){

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LABPROJECT11, szWindowClass, MAX_LOADSTRING);

	//<윈도우 클래스를 등록>
	MyRegisterClass(hInstance);

	// <2. 응용 프로그램 초기화를 수행합니다: 주 윈도우 생성을 수행한다.>
	if (!InitInstance(hInstance, nCmdShow))	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT11));

	MSG msg;

	// <메세지 루프>
	// 메세지 큐에서 메세지를 가져와서 메세지를 해당 윈도우에게 전달하는 DispatchMessage메세지 루프를 나타낸다 

	/*while (GetMessage(&msg, nullptr, 0, 0))*/
	// false: WM_QUIT--> 응용 프로그램을 종료하는 경우에 발생한다.
	// 다른 메세지 일 경우에는 true를 반환한다.
	while (1) {
	
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))		{
			if (msg.message == WM_QUIT) break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				// 이 윈도우 API가 호출하면 WndProc()함수가 실행된다.
			}
		}
		else{
			gGameFrameWork.FrameAdvance();
		}
	}
	gGameFrameWork.OnDestory();

	return (int)msg.wParam;
}




//  함수: MyRegisterClass()
//  용도: 창 클래스를 등록합니다.
//  설명: 윈도우 클래스는 윈도우의 특성/모양(class style)과 동작 방식(window precedure)을 나타낸다.
//  일반적으로 윈도우 응용프로그램이 실행되면 응용 프로그램을 대표하는 주 윈도우가 나타나고 이 윈도우의 모양과 동작 방식은
//  다른 응용 프로그램과 차별화되어야 한다. 적어도 윈도우 프로시져는 달라야 한다.
ATOM MyRegisterClass(HINSTANCE hInstance){
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc; // 윈도우 프로시져를 설정한다.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT11));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LABPROJECT11);
	wcex.lpszMenuName = NULL; //메뉴를 없애려면
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//   설명: 주 윈도우를 생성하고 화면에 보이도록 하는 함수이다. 윈도우를 생성하는 윈도우 API 함수는 CreateWindow()
//   주석: 이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고 주 프로그램 창을 만든 다음 표시합니다.

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
	// 윈도우 설정
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_BORDER | WS_SYSMENU ;
	RECT rc = { 0,0,FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT};
	AdjustWindowRect(&rc, dwStyle, FALSE);

	ghAppInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	
	// 윈도우 설정으로 윈도우 생성
	HWND hMainWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

	if (!hMainWnd) { return FALSE; }

	// 프로그램의 주 윈도우가 생성되면 OnCreate()함수를 호출하여 프레임 위크 객체 초기화 가즈아 !
	gGameFrameWork.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

//#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE  //풀 스크린 상태이면 swapchainstate변경 시켜라
//	gGameFrameWork.ChangeSwapChainState();
//#endif

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//  언제 실행되는데: 주 윈도우가 처리해야 하는 윈도우 메시지가 주 윈도우에게 전달될 때 호출되는 메시지 처리 함수
//  wWinMain -> DispatchMessage --> WndProc
//  용도: 주 창의 메시지를 처리합니다.
//  hWnd: 윈도우 핸들, message: 메시지 ID
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
// 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){

	case WM_COMMAND:{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId){
		case IDM_ABOUT:
			DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFrameWork.OnProcessingWindowMessage(hWnd, message, wParam, lParam); break;
	case WM_DESTROY:
		PostQuitMessage(0); //WM_QUIT메세지를 생성하여 메세지 큐에 삽입 한다.
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message){
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
