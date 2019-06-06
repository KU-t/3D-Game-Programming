#pragma once
#include "Timer.h"
#include "Scene.h"

class GameFramework{
private:

	GameTimer m_GameTimer;//���� �����ӿ�ũ���� ����� Ÿ�̸�
	_TCHAR m_pszFrameRate[50]; //������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ�

	// Scene
	Scene *m_pScene;

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;
	IDXGISwapChain3* m_pdxgiSwapChain; // ���÷��� ��� ����
	ID3D12Device* m_pd3dDevice; // ���ҽ� ������ ����

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; // ���� ���ø��� Ȱ��ȭ, ���� ���ø� ������ ����

	static const UINT m_nSwapChainBuffers = 2; //����ü�� ����� ��
	UINT m_nSwapChainBufferIndex; //����ü�� ����� ���� �ε���

	//���� Ÿ��, ������ ��, ���� Ÿ�� ������ ������ ũ��
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	//�������ٽ� ����, ������ ��, �������ٽ� ������ ������ ũ��
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;

	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽�
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;

	//�׷��� ���������� ����
	ID3D12PipelineState *m_pd3dPipelineState;

	//�潺 �������̽�
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	//if~endif: if�� ������ ���̸� endif���� �������� ����ȴ�.
#if defined(_DEBUG) 
	ID3D12Debug *m_pd3dDebugController;
#endif
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect; //����Ʈ�� ���� �簢���̴�. 

public:
	GameFramework();
	~GameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// �� �����찡 �����Ǹ� ȣ��ȴ�.

	void OnDestory();
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	// ����ü��, ����̽�, ������ ��, ��� ť, �Ҵ���, ����Ʈ�� ����

	void CreateRenderTargetViews();
	void CreateDepthStencilViews();


	void BuildObjects();
	void ReleaseObjects();
	// �������� �޽��� ���� ��ü�� �����ϰ� �Ҹ�

	void ProcessInput();
	void AnimateObject();
	void FrameAdvance();
	// ��������ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ�

	void WaitForGpuComplete(); //CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�. 

	//[�����ϱ�5 fullscreenmode]
	void ChangeSwapChainState();
	
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ�
};
