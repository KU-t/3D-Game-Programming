#pragma once
#include "Timer.h"
#include "Scene.h"

class GameFramework{
private:

	GameTimer m_GameTimer;//게임 프레임워크에서 사용할 타이머
	_TCHAR m_pszFrameRate[50]; //프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열

	// Scene
	Scene *m_pScene;

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;
	IDXGISwapChain3* m_pdxgiSwapChain; // 디스플레이 제어를 위해
	ID3D12Device* m_pd3dDevice; // 리소스 생성을 위해

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; // 다중 샘플링을 활성화, 다중 샘플링 레벨을 설정

	static const UINT m_nSwapChainBuffers = 2; //스왑체인 백버퍼 수
	UINT m_nSwapChainBufferIndex; //스왑체인 백버퍼 현재 인덱스

	//렌더 타켓, 서술자 힙, 렌더 타켓 서술자 원소의 크기
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	//디프스텐실 버퍼, 서술자 힙, 디프스텐실 서술자 원소의 크기
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
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
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect; //뷰포트와 씨저 사각형이다. 

public:
	GameFramework();
	~GameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// 주 윈도우가 생성되면 호출된다.

	void OnDestory();
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	// 스왑체인, 디바이스, 서술자 힙, 명령 큐, 할당자, 리스트를 생성

	void CreateRenderTargetViews();
	void CreateDepthStencilViews();


	void BuildObjects();
	void ReleaseObjects();
	// 렌더링할 메쉬와 게임 객체를 생성하고 소멸

	void ProcessInput();
	void AnimateObject();
	void FrameAdvance();
	// 프레임위크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수

	void WaitForGpuComplete(); //CPU와 GPU를 동기화하는 함수이다. 

	//[따라하기5 fullscreenmode]
	void ChangeSwapChainState();
	
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 윈도우의 메시지(키보드, 마우스 입력)을 처리하는 함수
};
