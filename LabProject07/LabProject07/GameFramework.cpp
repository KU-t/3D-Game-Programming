// 디바이스를 생성하고 관리하며 화면 출력을 위한 여러 가지 처리 
// (게임 객체의 생성과 관리, 사용자 입력, 애니메이션 등)를 담당한다.

#include "stdafx.h"
#include "GameFramework.h"


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

	// SwapChain Desc -  모니터 해상도에 맞게 확대하는 방법
	//dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_SCALING_NONE; 
	// SwapChain Desc - (뭐지?)
	//dxgiSwapChainDesc.BufferDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

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
	m_pd3dDevice->CheckFeatureSupport
	(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels,
		sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다. 
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다. 
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	//m_nFenceValue = 0; //펜스를 생성하고 펜스 값을 0으로 설정한다.
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다).
	  이벤트가 실행되면(Signal) 이 벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);


	//뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다.
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	//씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다.
	m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };

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

	// 직접(Direct) 명령 큐를 생성한다.
	// CommandQueue Desc, uuidof, CommandQueue 저장할 주소
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);
	
	// 직접(Direct) 명령 할당자를 생성한다.
	// Command Allocator - 유형 : GPU 직접 실행 가능한 명령 버퍼 (D3D12_COMMAND_LIST_TYPE_DIRECT)
	// 유형, uuidof, Command Allocator 저장할 주소
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	
	// 직접(Direct) 명령 리스트를 생성한다.
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

void GameFramework::CreateDepthStencilViews(){
	// Resource Desc 생성
	D3D12_RESOURCE_DESC d3dResourceDesc;
	// Resource Desc - 
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 힙 생성
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;


	//깊이-스텐실 버퍼를 생성한다.
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	//깊이-스텐실 버퍼 뷰를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// 힙의 내용을 버퍼로
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void GameFramework::FrameAdvance(){
	m_GameTimer.Tick(0.0f);
	
	ProcessInput();
	
	AnimateObject();

	//명령 할당자와 명령 리스트를 리셋한다.
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//뷰포트와 씨저 사각형을 설정한다.
	m_pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	m_pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);

	/*현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다.
	프리젠트가 끝나면 렌더 타겟 버퍼의 상태는 프리젠트 상태
	(D3D12_RESOURCE_STATE_PRESENT)에서 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바 뀔 것이다.*/
	D3D12_RESOURCE_BARRIER d3dResourceBarrier; //리소스 장벽[선미]
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	//현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	//깊이-스텐실 서술자의 CPU 주소를 계산한다. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM)에 연결한다.
	//렌더링 코드는 여기에 추가될 것이다.
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	//원하는 색상으로 렌더 타겟(뷰)을 지운다.
	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);

	//원하는 값으로 깊이-스텐실(뷰)을 지운다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	if (m_pScene) m_pScene->Render(m_pd3dCommandList);

	
	/*현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다. GPU가 렌더 타겟(버퍼)을
	더 이상 사용하지 않으면 렌더 타겟 의 상태는 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.*/
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	//명령 리스트를 닫힌 상태로 만든다.
	hResult = m_pd3dCommandList->Close(); 

	//명령 리스트를 명령 큐에 추가하여 실행한다.
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists)/*1*/, ppd3dCommandLists);
	
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	WaitForGpuComplete(); 

	///*스왑체인을 프리젠트한다. 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의
	//내용이 전면버퍼로 옮겨지고 렌더 타겟 인 덱스가 바뀔 것이다.*/
	//DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	//dxgiPresentParameters.DirtyRectsCount = 0;
	//dxgiPresentParameters.pDirtyRects = NULL;
	//dxgiPresentParameters.pScrollRect = NULL;
	//dxgiPresentParameters.pScrollOffset = NULL;
	
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
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
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

	//Scene 생성 -> Scene 객체 생성
	m_pScene = new Scene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

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
}

void GameFramework::AnimateObject(){
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());

}


// [따라하기 5번 fullscreenmode ]
void GameFramework::ChangeSwapChainState(){
	WaitForGpuComplete(); //gpu가 끝날때까지 기다려줘
	
	BOOL bFullScreenstate = false; 
	// 현재 상태 받아와
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenstate, nullptr);
	// 현재상태의 반대상태로 설정
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenstate, nullptr); 

	// 
	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	dxgiTargetParameters.Width = m_nWndClientWidth; 
	dxgiTargetParameters.Height = m_nWndClientHeight; 
	dxgiTargetParameters.RefreshRate.Numerator = 60; 
	dxgiTargetParameters.RefreshRate.Denominator = 1; 
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppd3dRenderTargetBuffers[i]) m_ppd3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc; 
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc); 
	m_pdxgiSwapChain->ResizeBuffers
	(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, 
		dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void GameFramework::MoveToNextFrame() {
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE); 
	}
}

