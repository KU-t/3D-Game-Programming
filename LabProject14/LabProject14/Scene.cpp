#include "stdafx.h"
#include "Scene.h"

#define _WITH_TERRAIN_PARTITION

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

	// 지형 그리기
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다.
	XMFLOAT3 xmf3Scale(16.0f, 2.0f, 16.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다.
#ifdef _WITH_TERRAIN_PARTITION 
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다.
	지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메 쉬를 가진다.
	지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("heightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else 
	//지형을 하나의 격자 메쉬(257x257)로 생성한다.
	m_pTerrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("heightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif


	//CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

	// 1종류의 객체를 가리키는 1개의 쉐이더
	m_nShaders = 1;
	// 1개의 쉐이더 주소 저장 메모리 할당 - m_pShaders는 m_nShaders만큼의 객체를 주소 배열로 관리
	m_pShaders = new ObjectsShader[m_nShaders];

	// Shader 생성
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	// 생성된 Shader의 객체 생성
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
}


void Scene::ReleaseObjects() {
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	
	for (int i = 0; i < m_nShaders; i++) {
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	
	if (m_pShaders)
		delete[] m_pShaders;

	if (m_pTerrain)
		delete m_pTerrain;

}


bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool Scene::ProcessInput(UCHAR *pKeysBuffer) {
	return false;
}

void Scene::AnimateObjects(float fTimeElapsed) {
	for (int i = 0; i < m_nShaders; i++) 
		m_pShaders[i].AnimateObjects(fTimeElapsed);
}

void Scene::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera){

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	// 그래픽 루트 시그너쳐를 파이프라인에 연결
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera)
		pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pTerrain)
		m_pTerrain->Render(pd3dCommandList, pCamera);


	// 씬을 구성하는 쉐이더(쉐이더에 포함된 객체)들을 렌더링
	for (int i = 0; i < m_nShaders; i++) 
		m_pShaders[i].Render(pd3dCommandList, pCamera); 
}

void Scene::ReleaseUploadBuffers() {
	for (int i = 0; i < m_nShaders; i++)
		m_pShaders[i].ReleaseUploadBuffers();

	if (m_pTerrain)
		m_pTerrain->ReleaseUploadBuffers();

}

ID3D12RootSignature *Scene::GetGraphicsRootSignature() {
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature *Scene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) {
	// [루트 시그니쳐 - 할당받을 포인터 생성] : 파이프라인의 전역변수
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	// Root Parameters 생성 : 2개
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	// 1번 Root Parameter
	// 1번 Root Parameter - 유형 : 루트 상수
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	// 1번 Root Parameter - 상수 개수 : 16개
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	// 1번 Root Parameter - 레지스터 번호 : 0
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	// 1번 Root Parameter - 레지스터 공간 : 0
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	// 1번 Root Parameter - 어떤 Shader에서 사용 가능한가? : Vertex Shader
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// 2번 Root Parameter
	// 2번 Root Parameter - 유형 : 루트 상수
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	// 2번 Root Parameter - 상수 개수 : 32개
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	// 2번 Root Parameter - 레지스터 번호 : 0
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	// 2번 Root Parameter - 레지스터 공간 : 0
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	// 2번 Root Parameter - 어떤 Shader에서 사용 가능한가? : Vertex Shader
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// 루트 시그니처 레이아웃(어떤 쉐이더에서 접근 가능한가.)
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// [루트 시그니처 - 설정]
	// 루트 시그니처 DESC 생성
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	// 루트 시그니처 DESC - 모든 변수 초기화 0 or NULL
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	// 루트 시그니처 DESC - root parameter 개수 : 2개
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	// 루트 시그니처 DESC - root parameter 시작 주소 : pd3dRootParameters
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	// 루트 시그니처 DESC - static sampler 개수 (정점 샘플러의 개수)
	d3dRootSignatureDesc.NumStaticSamplers = 0; 
	// 루트 시그니처 DESC - static sampler 주소
	d3dRootSignatureDesc.pStaticSamplers = NULL; 
	// 루트 시그니처 DESC - 레이아웃(어떤 쉐이더에서 접근 가능한가.) : IA단계를 허용
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	// Blob 생성
	ID3DBlob *pd3dSignatureBlob = NULL; 
	ID3DBlob *pd3dErrorBlob = NULL;

	// 생성한 루트 시그니처 DESC의 설정, 버전, Blob을 이용해 (할당 ?)
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	// pd3dDevice에 RootSignature생성 : (단일 GPU, 직렬화된 RootSignature의 데이터 포인터, RootSignature의 크기, riid, 생성할 RootSignature의 주소)
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),(void **)&pd3dGraphicsRootSignature);
	
	// com객체 Release
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	
	// 생성한 RootSignature의 주소를 내보내
	return pd3dGraphicsRootSignature;
}

