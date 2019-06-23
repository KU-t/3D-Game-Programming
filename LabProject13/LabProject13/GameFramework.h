#pragma once
#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class GameFramework{
private:

	//게임 프레임워크에서 사용할 타이머
	GameTimer m_GameTimer;
	//프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열
	_TCHAR m_pszFrameRate[50]; 

	// Scene
	Scene *m_pScene;
	// Camera
	Camera *m_pCamera = NULL;
	// Player
	Player *m_pPlayer = NULL;

	// 마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치
	POINT m_ptOldCursorPos;    

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;
	// 디스플레이 제어를 위해
	IDXGISwapChain3* m_pdxgiSwapChain;
	// 리소스 생성을 위해
	ID3D12Device* m_pd3dDevice;

	// 다중 샘플링을 활성화
	bool m_bMsaa4xEnable = false;
	// 다중 샘플링 레벨을 설정
	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2; //스왑체인 백버퍼 수
	//스왑체인 백버퍼 현재 인덱스
	UINT m_nSwapChainBufferIndex; 

	// 렌더 타겟 버퍼
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// 렌더 타켓 서술자 힙 주소
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	// 렌더 타켓 서술자 원소의 크기
	UINT m_nRtvDescriptorIncrementSize;

	// 깊이-스텐실 버퍼
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	// 디프스텐실 서술자 힙 주소
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	// 디프스텐실 서술자 원소의 크기
	UINT m_nDsvDescriptorIncrementSize;

	//명령 큐, 명령 할당자, 명령 리스트 인터페이스
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;

	//그래픽 파이프라인 상태
	ID3D12PipelineState *m_pd3dPipelineState;

	//펜스 인터페이스
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	//if~endif: if의 내용이 참이면 endif까지 컴파일이 진행된다.
#if defined(_DEBUG) 
	ID3D12Debug *m_pd3dDebugController;
#endif

public:
	GameFramework();
	~GameFramework();

	// 주 윈도우가 생성되면 호출된다.
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void OnDestory();

	// 스왑체인 생성
	void CreateSwapChain();
	// 디바이스 생성
	void CreateDirect3DDevice();
	// 서술자 힙 생성
	void CreateRtvAndDsvDescriptorHeaps();
	// 명령 큐, 할당자, 리스트 생성
	void CreateCommandQueueAndList();
	// 렌더 타겟 뷰 생성
	void CreateRenderTargetViews();
	// 깊이 - 스텐실 뷰 생성
	void CreateDepthStencilViews();

	// 렌더링할 메쉬와 게임 객체를 생성
	void BuildObjects();
	// 렌더링할 메쉬와 게임 객체를 소멸
	void ReleaseObjects();

	// 프레임위크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수
	void ProcessInput();
	void AnimateObject();
	void FrameAdvance();

	// CPU와 GPU를 동기화하는 함수이다. 
	void WaitForGpuComplete(); 

	//[따라하기5 fullscreenmode]
	void ChangeSwapChainState();
	
	void MoveToNextFrame();

	// 윈도우의 메시지(키보드, 마우스 입력)을 처리하는 함수
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
