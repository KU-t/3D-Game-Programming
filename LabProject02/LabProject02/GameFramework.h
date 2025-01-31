#pragma once
class GameFramework{
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4 *m_pdxgiFactory;	// DXGI 팩토리 인터페이스에 대한 포인터
	IDXGISwapChain3 *m_pdxgiSwapChain;	// 스왑 체인 인터페이스에 대한 포인터 ( 디스플레이 제어)
	ID3D12Device *m_pd3dDevice;		// Direct3D 디바이스 인터페이스에 대한 포인터 (리소스 생성)

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2;	// 스왑 체인의 후면 버퍼의 개수
	UINT m_nSwapChainBufferIndex;		// 현재 스왑 체인의 후면 버퍼 인덱스

	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];	// 렌더 타겟버퍼 포인터
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;	// 서술자 힙 인터페이스 포인터 
	UINT m_nRtvDescriptorIncrementSize;	// 렌더 타겟 서술자 원소 크기

	ID3D12Resource *m_pd3dDepthStencilBuffer;	// 깊이-스텐실 버퍼 포인터
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;	// 서술자 힙 인터페이스 포인터
	UINT m_nDsvDescriptorIncrementSize;	// 깊이-스텐실 서술자 원소의 크기

	ID3D12CommandQueue *m_pd3dCommandQueue;	// 명령 큐 포인터
	ID3D12CommandAllocator *m_pd3dCommandAllocator;	// 명령 할당자 포인터
	ID3D12GraphicsCommandList *m_pd3dCommandList;	// 명령 리스트 인터페이스 포인터

	ID3D12PipelineState *m_pd3dPipelineState;	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터

	ID3D12Fence *m_pd3dFence;	// 펜스 인터페이스 포인터
	UINT64 m_nFenceValue;	// 펜스의 값
	HANDLE m_hFenceEvent;	// 이벤트 핸들

#if defined (_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif

	D3D12_VIEWPORT m_d3dViewport;	// 뷰포트
	D3D12_RECT m_d3dScissorRect;	//씨저 사각형

public:
	GameFramework();
	~GameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	// 프레임워크를 초기화 하는 함수
	void OnDestroy();

	void CreateSwapChain();	// 스왑체인 생성함수
	void CreateDirect3DDevice();	// 디바이스 생성함수
	void CreateRtvAndDsvDescriptorHeaps();	// 서술자 힙 생성함수
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList(); // 명령 큐/할당자/리스트 생성함수

	// 렌더링할 메쉬 게임 객체 
	void BuildObjects();	// 생성
	void ReleaseObjects();	// 제거

	// 프레임워크 핵심
	void ProcessInput();	// 사용자 입력
	void AnimateObjects();	// 애니메이션
	void FrameAdvance();	// 렌더링

	void WaitForGpuComplete();	// CPU - GPU 동기화 함수

	// 윈도우 메시지 (마우스, 키보드 입력) 처리함수
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


};

