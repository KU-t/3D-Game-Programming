#include "stdafx.h"
#include "Scene.h"


Scene::Scene() {
	//[Lab05]
	//m_pd3dPipelineState = NULL;
	//m_pd3dGraphicsRootSignature = NULL;
}


Scene::~Scene() {

}

//[Lab06]
void Scene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {

	// GraphicsRootSignature를 (GameFramework)pd3dDevice에 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// Scene을 그리기 위한 Shader 객체를 생성
	// Scene은 총 1개
	m_nShaders = 1;
	// 1개의 Shader를 생성할 포인터 생성 - m_ppShaders는 m_nShaders개수만큼의 Shader를 배열로 관리
	m_ppShaders = new Shader*[m_nShaders];

	// 새로운 하나의 Shader를 생성
	Shader *pShader = new Shader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	// Shader의 객체 생성
	pShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	// Shader를 Shader관리 포인터에 추가
	m_ppShaders[0] = pShader;
}


void Scene::ReleaseObjects() {
	//if (m_pd3dPipelineState) m_pd3dPipelineState->Release();

	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppShaders) {
		for (int i = 0; i < m_nShaders; i++) {
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
}


bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool Scene::ProcessInput() {
	return false;
}

void Scene::AnimateObjects(float fTimeElapsed) {
	for (int i = 0; i < m_nShaders; i++) {
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}

}

void Scene::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	//그래픽 루트 시그너쳐를 파이프라인에 연결(설정)한다.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//씬을 렌더링하는 것은 씬을 구성하는 셰이더(셰이더가 포함하는 객체)들을 렌더링하는 것이다.
	for (int i = 0; i < m_nShaders; i++) {
		m_ppShaders[i]->Render(pd3dCommandList);
	}
}

void Scene::ReleaseUploadBuffers() {
	if (m_ppShaders) {
		for (int j = 0; j < m_nShaders; j++)
			if (m_ppShaders[j]) m_ppShaders[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature *Scene::GetGraphicsRootSignature() {
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature *Scene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) {
	// [루트 시그니쳐 - 할당받을 포인터 생성] : 파이프라인의 전역변수
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	// [루트 시그니처 - 설정]
	// 루트 시그니처 DESC 생성
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	// 루트 시그니처 DESC - 모든 변수 초기화 0 or NULL
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	// 루트 시그니처 DESC - root parameter 개수
	d3dRootSignatureDesc.NumParameters = 0;
	// 루트 시그니처 DESC - root parameter 주소
	d3dRootSignatureDesc.pParameters = NULL;
	// 루트 시그니처 DESC - static sampler 개수 (정점 샘플러의 개수)
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	// 루트 시그니처 DESC - static sampler 주소
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	// 루트 시그니처 DESC - 레이아웃(어떤 쉐이더에서 접근 가능한가.) : IA단계를 허용
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Blob 생성 (뭐하는 애지?)
	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;

	// 생성한 루트 시그니처 DESC의 설정, 버전, Blob을 이용해 (할당 ?)
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	// pd3dDevice에 RootSignature생성 : (단일 GPU, 직렬화된 RootSignature의 데이터 포인터, RootSignature의 크기, riid, 생성할 RootSignature의 주소)
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);

	// com객체 Release
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	// 생성한 RootSignature의 주소를 내보내
	return pd3dGraphicsRootSignature;
}

