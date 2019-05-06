#pragma once
#include "GameTimer.h"
#include "Scene.h"

class GameFramework {
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	
	//scene 
	CScene *m_pScene;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	// DXGI ���丮 �������̽��� ���� ������
	IDXGIFactory4 *m_pdxgiFactory;	
	// ���� ü�� �������̽��� ���� ������ ( ���÷��� ����)
	IDXGISwapChain3 *m_pdxgiSwapChain;	
	// Direct3D ����̽� �������̽��� ���� ������ (���ҽ� ����)
	ID3D12Device *m_pd3dDevice;		

	//���߻��ø� 4�� ����
	bool m_bMsaa4xEnable = false;
	//���߻��ø� MSDI���� ����
	UINT m_nMsaa4xQualityLevels = 0;

	// ���� ü���� �ĸ� ������ ����
	static const UINT m_nSwapChainBuffers = 2;	
	// ���� ���� ü���� �ĸ� ���� �ε���
	UINT m_nSwapChainBufferIndex;		

	// ���� Ÿ�ٹ��� ������
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];	
	// ������ �� �������̽� ������ 
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;	
	// ���� Ÿ�� ������ ���� ũ��
	UINT m_nRtvDescriptorIncrementSize;	

	// ����-���ٽ� ���� ������
	ID3D12Resource *m_pd3dDepthStencilBuffer;	
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;	
	// ����-���ٽ� ������ ������ ũ��
	UINT m_nDsvDescriptorIncrementSize;	

	// ��� ť ������
	ID3D12CommandQueue *m_pd3dCommandQueue;	
	// ��� �Ҵ��� ������
	ID3D12CommandAllocator *m_pd3dCommandAllocator;	
	// ��� ����Ʈ �������̽� ������
	ID3D12GraphicsCommandList *m_pd3dCommandList;	

	// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� ������
	ID3D12PipelineState *m_pd3dPipelineState;	

	// �潺 �������̽� ������
	ID3D12Fence *m_pd3dFence;	
	// �ĸ���� �潺��
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	// �̺�Ʈ �ڵ�
	HANDLE m_hFenceEvent;	

	

	

#if defined (_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif

	// ����Ʈ
	D3D12_VIEWPORT m_d3dViewport;	

	//���� �簢��
	D3D12_RECT m_d3dScissorRect;	

	//���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	GameTimer m_GameTimer;

	//������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszFrameRate[50];

public:
	GameFramework();
	~GameFramework();

	// �����ӿ�ũ�� �ʱ�ȭ �ϴ� �Լ�
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	
	
	// �����ӿ�ũ ����
	void OnDestroy();

	// ����ü�� �����Լ�
	void CreateSwapChain();	

	// ����̽� �����Լ�
	void CreateDirect3DDevice();	

	// ������ �� �����Լ�
	void CreateRtvAndDsvDescriptorHeaps();	
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	// ��� ť/�Ҵ���/����Ʈ �����Լ�
	void CreateCommandQueueAndList(); 

	// �������� �޽� ���� ��ü 
	void BuildObjects();	// ����
	void ReleaseObjects();	// ����

	// �����ӿ�ũ �ٽ�
	void ProcessInput();	// ����� �Է�
	void AnimateObjects();	// �ִϸ��̼�
	void FrameAdvance();	// ������

	// CPU - GPU ����ȭ �Լ�
	void WaitForGpuComplete();	

	// ����ü�� ����Լ�
	void ChangeSwapChainState();

	// ������
	void MoveToNextFrame();

	// ������ �޽��� (���콺, Ű���� �Է�) ó���Լ�
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


};

