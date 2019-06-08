#pragma once
#include "Timer.h"
#include "GameObject.h"
#include "Camera.h"

class Scene{
public:
	Scene();
	~Scene();

	// 마우스 입력처리
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 키보드 입력처리
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera);

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();

protected: 
	//씬은 게임 객체들의 집합이다. 게임 객체는 셰이더를 포함한다.
	GameObject **m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};


