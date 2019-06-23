// 디바이스를 생성하고 관리하며 화면 출력을 위한 여러 가지 처리 
// (게임 객체의 생성과 관리, 사용자 입력, 애니메이션 등)를 담당한다.

#include "stdafx.h"
#include "GameFramework.h"

#define _WITH_PLAYER_TOP

GameFramework::GameFramework() {
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dPipelineState = NULL;
	m_pd3dCommandList = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) {
		m_ppd3dRenderTargetBuffers[i] = NULL;
		m_nFenceValues[i] = 0;
	}
	m_pScene = nullptr;

	m_pd3dRtvDescriptorHeap = NULL;
	m_nRtvDescriptorIncrementSize = 0;

	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDsvDescriptorHeap = NULL;
	m_nDsvDescriptorIncrementSize = 0;

	m_nSwapChainBufferIndex = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("LapProject ("));
	// _tcscpy_s(char *_Dst, rsize _t _SizeInBytes, const char* _Src)
	// _tcscpy_s() 함수 사용의 주 목적:버퍼 Overflow를 막거나, 잘못된 메모리 연산 방지를 위해
	// 사용 방법 및 예제 tcscpy_s(저장 변수, 쓰려고 하는 데이터 크기 + 1, 쓰려고 하는 데이터)

}


GameFramework::~GameFramework() {
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) {
// 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출 !!!

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	// Gameframework 생성
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilViews();

	// 게임 오브젝트 생성
	BuildObjects();

	return true;
}


void GameFramework::OnDestory(){
	ReleaseObjects();
	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG) 
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator)m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

}

inline void GameFramework::CreateSwapChain() {

	// 클라이언트 사각형 정보 가져오기
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	// 클라이언트 사각형 정보 저장
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	// SwapChain Desc 생성
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	// SwapChain Desc - 초기화
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	// SwapChain Desc - 크기
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	// SwapChain Desc - 후면버퍼 디스플레이 형식
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// SwapChain Desc - 다중 샘플링 품질 지정
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	// SwapChain Desc - 후면버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 : 'CPU도 후면버퍼에 접근 가능'
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
	// SwapChain Desc - 후면버퍼의 수를 지정
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	// SwapChain Desc - 스와핑을 처리하는 선택사항 : 버퍼 내용물 폐기
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 
	// SwapChain Desc - 스왑체인의 동작에 대한 선택사항을 지정 : 응용프로그램이 디스플레이 모드 변경가능
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	//[따라하기5: 전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.]
	// SwapChain Desc - 윈도우, 전체화면 모드지정
	dxgiSwapChainDesc.Windowed = TRUE;
	// SwapChain Desc - 프레임 : 60Hz (1초에 60번)
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60; 
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	// SwapChain Desc - 출력 윈도우의 핸들
	dxgiSwapChainDesc.OutputWindow = m_hWnd;

	// Swap Chain 생성
	// Command Queue, SwapChain Desc, 저장할 주소
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);

	// 스왑체인의 현재 후면버퍼 인덱스를 저장
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	//“Alt+Enter” 키의 동작을 비활성화한다.  [선미]
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE 
	CreateRenderTargetViews(); 
#endif 

	// Swap Chain Full Screen Desc 생성
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	// Swap Chain Full Screen Desc - 초기화
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	// Swap Chain Full Screen Desc - 프레임 : 60Hz 
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	// Swap Chain Full Screen Desc - 스캔라인 그리기 모드 지정 : 스캔 라인 순서를 지정하지 않음
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// Swap Chain Full Screen Desc - 모니터 해상도에 맞게 확대방법 : 알아서 해
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Swap Chain Full Screen Desc - 윈도우, 전체화면 모드지정 : 가능
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;


	//m_pdxgiFactory->CreateSwapChainForHwnd //스왑체인을 생성한다. 
	//(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc,
	//	NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);

	//“Alt+Enter” 키의 동작을 비활성화한다.  [선미]
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	// 스왑체인의 현재 후면버퍼 인덱스를 저장
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void GameFramework::CreateDirect3DDevice() {

#if defined(_DEBUG) 
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif
	// DXGI Factory 생성 - m_pdxgiFactory로
	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	// Adapter 메모리 할당
	IDXGIAdapter1 *pd3dAdapter = NULL;

	// Adapter의 개수 가져오기
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++) {
		// Adapter Desc 생성
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		// Adapter Desc - Adapter 의 능력 가져오기
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);

		// 능력이 아니라면 skip
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		// 디바이스를 생성하는데 성공했다면,
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device),(void **)&m_pd3dDevice)))
			break;
	}

	// 가상 디바이스 생성
	if (!pd3dAdapter){
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	//[선미]특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//Msaa4x 다중 샘플링 
	d3dMsaaQualityLevels.SampleCount = 4;

	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다. 
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다. 
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	//m_nFenceValue = 0; //펜스를 생성하고 펜스 값을 0으로 설정한다.
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	
	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다).
	  이벤트가 실행되면(Signal) 이 벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);



	if (pd3dAdapter) pd3dAdapter->Release();
}


void GameFramework::CreateCommandQueueAndList(){
	// CommandQueue Desc 생성
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	// CommandQueue Desc - 초기화
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

	// CommandQueue Desc - 유형 : GPU가 직접 실행할수 있는 명령 버퍼(리스트)
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// CommandQueue Desc - 설정 : 기본 명령 큐
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// 직접(Direct) 명령 큐를 생성
	// CommandQueue Desc, uuidof, CommandQueue 저장할 주소
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);
	
	// 직접(Direct) 명령 할당자를 생성
	// Command Allocator - 유형 : GPU 직접 실행 가능한 명령 버퍼 (D3D12_COMMAND_LIST_TYPE_DIRECT)
	// 유형, uuidof, Command Allocator 저장할 주소
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	
	// 직접(Direct) 명령 리스트를 생성
	// 대상 어댑터 (GPU 하나면 0), 유형 : GPU 직접 실행 가능한 명령 버퍼, 명령어 리스트를 생성할 명령어 할당자, 초기 파이프라인 상태(NULL : 라이브러리가 알아서), uuidof, 저장 주소
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다. 
	hResult = m_pd3dCommandList->Close();
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps(){
	// Descriptor Heap Desc 생성
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	// Descriptor Heap Desc - 초기화
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	// Descriptor Heap Desc - 서술자의 개수 : Swap Chain Buffer의 개수
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	// Descriptor Heap Desc - 서술자 타입 : RTV (Rander Target View)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	// Descriptor Heap Desc - 설정 : 없어
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	// Descriptor Heap Desc - 어떤 GPU? : 0번만
	d3dDescriptorHeapDesc.NodeMask = 0;

	// 렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다. 
	// Descriptor Heap Desc, uuidof, 저장할 주소
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);

	// 렌더 타겟 서술자 힙의 원소의 크기를 저장한다.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Descriptor Heap Desc - 서술자의 개수 : 1개  (깊이- 스텐실 서술자)
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	// Descriptor Heap Desc - 서술자 타입 : DSV
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	// 깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다. 
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);

	// 깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}


void GameFramework::CreateRenderTargetViews(){
	// Rtv CPU Descriptor Handle 생성 - 힙의 시작을 나타내는 CPU 서술자 핸들 첫 주소를 받음
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// Swap Chain Buffer의 개수만큼 반복
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)	{
		// 스왑체인의 i번째 후면버퍼, uuidof를, Render Target Buffer[i]로 반환
		HRESULT hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource),(void **)&m_ppd3dRenderTargetBuffers[i]);

		// 새로 만들어진 d3dRtvCPUDescriptorHandle(주소)의 내용을 Render Target Buffer[i]로 만들어
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

// 깊이 - 스텐실 뷰 생성
void GameFramework::CreateDepthStencilViews(){
	// Resource Desc 생성
	D3D12_RESOURCE_DESC d3dResourceDesc;
	// Resource Desc - 리소스 종류 : 2D 텍스쳐
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// Resource Desc - 리소스가 어떻게 정렬되는가? : 0 == 64KB
	d3dResourceDesc.Alignment = 0;
	// Resource Desc - 가로 크기 : 클라이언트 가로 길이
	d3dResourceDesc.Width = m_nWndClientWidth;
	// Resource Desc - 세로 크기 : 클라이언트 세로 길이
	d3dResourceDesc.Height = m_nWndClientHeight;
	// Resource Desc - 깊이 또는 배열의 크기
	d3dResourceDesc.DepthOrArraySize = 1;
	// Resource Desc - 밉맵레벨
	d3dResourceDesc.MipLevels = 1;
	// Resource Desc - 리소스 형식 : 깊이 24비트 (부호화x 0 ~ 1), 스텐실 8비트 (부호 x 정수) 
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Resource Desc - 다중 샘플링 (픽셀마다 샘플 개수) : 하면 4, 안하면 1개
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	// Resource Desc - 다중 샘플링 : 하면 레벨 - 1, 품질 레벨 0 == 다중 샘플링 하지 않음
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	// Resource Desc - 다차원 리소스를 1차원 리소스로 매핑하기 위한 방법 : 어댑터에 의존
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// Resource Desc - 어떤 뷰에서 사용 가능한지 : DSV 허용
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// Resource Heap Desc 생성
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	// Resource Heap Desc - 초기화
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	// Resource Heap Desc - 힙의 유형 : GPU 읽고 쓰기, CPU 불가 (대부분의 리소스)
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	// Resource Heap Desc - 힙에 대한 CPU 페이지 속성 (CPU가 어떻게 접근 가능한가, 불가한가)
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// Resource Heap Desc - 힙에 대한 메모리 풀( CPU vs GPU 대역폭이 어디가 더 큰가 )
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	// Resource Heap Desc - GPU 어댑터 : 0 == 1개이상 // 1 == 1개
	d3dHeapProperties.CreationNodeMask = 1;
	// Resource Heap Desc - 다중 어댑터에서 리소스를 볼 수 있는 노드 집합 (어떤 GPU가 접근할수 있나?)
	d3dHeapProperties.VisibleNodeMask = 1;

	// 최적의 지우기 연산을 위해
	// Clear Value Desc 생성
	D3D12_CLEAR_VALUE d3dClearValue;
	// Clear Value Desc - 어떤 포멧인가
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Clear Value Desc - 깊이값 (0.0 ~ 1.0)
	d3dClearValue.DepthStencil.Depth = 1.0f;
	// Clear Value Desc - 스텐실 값 (0 ~ 255)
	d3dClearValue.DepthStencil.Stencil = 0;


	// Device에 깊이-스텐실 버퍼를 생성한다.
	// Heap Properties Desc 주소,  힙 옵션 : 없음, Resource Desc 주소, Resource State (상태), 연산 주소(?), 어떤 형태로, 저장할 주소
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	// 깊이-스텐실 서술자의 힙 시작 주소를 받는다
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	
	// 깊이-스텐실 버퍼에 깊이-스텐실 서술자 핸들을 넣는다
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void GameFramework::FrameAdvance(){
	m_GameTimer.Tick(0.0f);
	
	ProcessInput();
	
	AnimateObject();

	//명령 할당자와 명령 리스트를 리셋한다.
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// Resource Barrier 생성
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	// Resource Barrier - 초기화
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	// Resource Barrier - 유형 : 전이 장벽
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Resource Barrier - 리소스 장벽을 분리 하기 위한 설정 
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// Resource Barrier - 상태전이 대상 리소스 : 새로 할당한 렌더타겟 버퍼
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	// Resource Barrier - 이전 상태 : 'PRESENT를 목표로 사용'
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// Resource Barrier - 이후 상태 : '렌더 타겟을 목표로 사용'
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// Resource Barrier - 서브 리소스의 인덱스 : 모든 서브 리소스
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// Resource Barrier
	// 배열 원소의 개수, 배열 주소
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// 렌더 타겟에 해당하는 서술자의 CPU 시작 주소(핸들)를 받아
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// 현재의 렌더 타겟에 해당하는 주소를 계산한다.
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	// 원하는 색상으로 렌더 타겟 뷰를 지운다.
	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);
	
	//깊이-스텐실 서술자의 CPU 시작 주소를 받아온다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 원하는 값으로 깊이-스텐실(뷰)을 지운다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	// 렌더 타겟 뷰, 깊이-스텐실 뷰를 출력-병합 단계(OM)에 연결
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	// 렌더링
	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);


	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다.
#ifdef _WITH_PLAYER_TOP 
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif 

	//3인칭 카메라일 때 플레이어를 렌더링한다.
	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	
	/*현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다.*/

	// 렌더링 이후 Resource Barrier 변경
	// Resource Barrier - 이전 상태 : '렌더 타겟을 목표로 사용'
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// Resource Barrier - 이후 상태 : 'PRESENT를 목표로 사용'
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// Resource Barrier - 서브 리소스의 인덱스 : 모든 서브 리소스
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// Resource Barrier
	// 배열 원소의 개수, 배열 주소
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	// 명령 리스트를 닫힌 상태
	hResult = m_pd3dCommandList->Close(); 

	// 명령 리스트를 명령 큐에 추가
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists)/*1*/, ppd3dCommandLists);
	
	// GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	WaitForGpuComplete(); 
	
	// 렌더링 된 영상을 Present - 디스플레이 출력
	m_pdxgiSwapChain->Present(0, 0 /*&dxgiPresentParameters*/);

	MoveToNextFrame();

	/*::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate + 12), 37, 10);
	::wcscat_s((m_pszFrameRate + 12), 37, _T(" FPS)"));*/

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

	//m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}


void GameFramework::WaitForGpuComplete(){
	//CPU 펜스의 값을 증가한다. 
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다. 
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다. 
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent); ::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}


void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID)	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// 마우스 캡쳐 - 현재 마우스 위치를 가져온다.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos); 
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		// 마우스 캡쳐 - 해제
		::ReleaseCapture(); 
		break;
	case WM_MOUSEMOVE:
		break;
	default: break;
	}

}

void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID)	{
	case WM_KEYUP:
		switch (wParam)		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;

		case VK_RETURN:
			break;

		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer)
				m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;

		case VK_F8:
			break;

		case VK_F9:
			ChangeSwapChainState();
			break;
		default: break;
		}
		break;
	default: break;
	}

}

LRESULT GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID) {
	case WM_SIZE:	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	} 
	return(0);
}

void GameFramework::BuildObjects(){

	// 커맨드 리스트 생성
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// Scene 생성 -> Scene 객체 생성
	m_pScene = new Scene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	// Player 생성
	AirplanePlayer *pAirplanePlayer = new AirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
	// GameFramework의 Player를 동일하게
	m_pPlayer = pAirplanePlayer;
	// Player의 Camera를 GameFramework의 Camera로 
	m_pCamera = m_pPlayer->GetCamera();

	//씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다.
	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//그래픽 명령 리스트들이 모두 실행될 때까지 기다린다.
	WaitForGpuComplete();

	//그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다.
	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	
	//m_GameTimer.Reset();
}

void GameFramework::ReleaseObjects(){
	if (m_pScene) {
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}
}

void GameFramework::ProcessInput(){
	static UCHAR pKeyBuffer[256];

	DWORD dwDirection = 0;
	/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면
	플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/ 뒤(로컬 z-축)로 이동한다.
	‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
	
	if (::GetKeyboardState(pKeyBuffer)) { 
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT; 
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	
	float cxDelta = 0.0f, cyDelta = 0.0f;
	
	POINT ptCursorPos;
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다.
	마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의 메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 
	그러므로 마우스가 캡쳐된 것은 마우스 버튼이 눌려진 상태를 의미한다. 
	마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플 레이어를 x-축 또는 y-축으로 회전한다.*/ 
	
	if (::GetCapture() == m_hWnd) {
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다). 
		::SetCursor(NULL);
		
		//현재 마우스 커서의 위치를 가져온다.
		::GetCursorPos(&ptCursorPos);
		
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다. 
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		
		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다.
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y); 
	} 
	
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)) {
		if (cxDelta || cyDelta) { 
			/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다.
			오른쪽 마우스 버튼이 눌려진 경우 cxDelta는 z-축의 회전을 나타낸다.*/ 
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			
			else m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);        
		} 

		/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다).
		이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정한다.*/
		
		if (dwDirection) m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), true);
	}

	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다.
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed()); 
}

void GameFramework::AnimateObject(){
	if (m_pScene)
		m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}


// [따라하기 5번 fullscreenmode ]
void GameFramework::ChangeSwapChainState(){
	//gpu가 끝날때까지 기다려줘
	WaitForGpuComplete(); 
	
	BOOL bFullScreenstate = false; 
	// 현재 스크린 모드를 받아와
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenstate, nullptr);
	// 현재 스크린 모드의 반대로 설정
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenstate, nullptr); 

	// Swap Chain 크기 변경
	// Target Parameter Desc 생성
	DXGI_MODE_DESC dxgiTargetParameters;
	// Target Parameter Desc - 포멧 : RGBA (노말라이징x)
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	// Target Parameter Desc - 가로 크기 : 클라이언트 가로 크기로
	dxgiTargetParameters.Width = m_nWndClientWidth; 
	// Target Parameter Desc - 세로 크기 : 클라이언트 세로 크기로
	dxgiTargetParameters.Height = m_nWndClientHeight; 
	// Target Parameter Desc - 프레임 : 60Hz (1초에 60번)
	dxgiTargetParameters.RefreshRate.Numerator = 60; 
	dxgiTargetParameters.RefreshRate.Denominator = 1; 
	// Target Parameter Desc - 모니터 해상도에 맞게 확대방법 : 알아서 해
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Target Parameter Desc - 스캔라인 그리기 모드 지정 : 스캔 라인 순서를 지정하지 않음
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
	
	// 스왑체인이 윈도우 모드 일때, 윈도우의 크기 변경
	// 스왑체인이 전체 화면 모드 일때, 바탕화면 모드 변경
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	// 모든 렌더타겟 뷰 (com 객체) Release
	// 클리이언트 영역의 크기가 변경되면, DXGI가 알아서 전면버퍼 크기 변경
	// 후면버퍼 크기를 변경전에 후면버퍼의 참조를 해제해야함
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppd3dRenderTargetBuffers[i]) m_ppd3dRenderTargetBuffers[i]->Release();

	// 스왑체인의 크기 변경
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc; 
	// 현재 스왑체인을 받아
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc); 
	// 스왑체인의 버퍼 개수, 바꿀 클라이언트 가로 영역,세로 영역, 바꿀 후면버퍼의 형식(유지), 스왑체인 flag
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, 
		dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	// 현재 스왑체인의 인덱스를 받아
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	// 렌더 타겟을 변경된 값으로 다시 생성 
	CreateRenderTargetViews();
}

void GameFramework::MoveToNextFrame() {
	// 현재 스왑체인 버퍼의 인덱스를 받아
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	// 현재 스왑체인 버퍼에 해당하는 펜스를 넘기고 1을 더해 둬
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	// 현재 펜스값을 설정하는 명령큐를 마지막에 넣어
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	// 현재의 펜스값을 받아 스왑체인의 펜스값 보다 작을때,
	if (m_pd3dFence->GetCompletedValue() < nFenceValue) {
		// 스왑체인의 값이 되면, Event를 깨워
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE); 
	}
}

