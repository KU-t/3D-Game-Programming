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
	
	// GraphicsRootSignature�� (GameFramework)pd3dDevice�� ����
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

	// ��ü 1��
	m_nObjects = 1;
	// 1���� ��ü �ּ� ���� �޸� �Ҵ� - m_ppObjects�� m_nObjects��ŭ�� ��ü�� �ּ� �迭�� ����
	m_ppObjects = new GameObject*[m_nObjects];

	// ȸ���ϴ� ��ü ����
	RotatingObject *pRotatingObject = new RotatingObject();
	// ȸ���ϴ� ��ü ����
	pRotatingObject->SetMesh(pCubeMesh);

	// Shader ����
	DiffusedShader *pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	pRotatingObject->SetShader(pShader);

	m_ppObjects[0] = pRotatingObject;

}


void Scene::ReleaseObjects() {
	//if (m_pd3dPipelineState) m_pd3dPipelineState->Release();

	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				delete m_ppObjects[j];

		delete[] m_ppObjects;
	}

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
	for (int j = 0; j < m_nObjects; j++) {
		m_ppObjects[j]->Animate(fTimeElapsed);
	}

}

void Scene::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera){

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);

	//���� �������ϴ� ���� ���� �����ϴ� ���� ��ü(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�.
	for (int j = 0; j < m_nObjects; j++) {
		if (m_ppObjects[j])
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	} 
}

void Scene::ReleaseUploadBuffers() {
	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature *Scene::GetGraphicsRootSignature() {
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature *Scene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) {
	// [��Ʈ �ñ״��� - �Ҵ���� ������ ����] : ������������ ��������
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	// Root Parameters ���� : 2��
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	// 1�� Root Parameter
	// 1�� Root Parameter - ���� : ��Ʈ ���
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	// 1�� Root Parameter - ��� ���� : 16��
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	// 1�� Root Parameter - �������� ��ȣ : 0
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	// 1�� Root Parameter - �������� ���� : 0
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	// 1�� Root Parameter - � Shader���� ��� �����Ѱ�? : Vertex Shader
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// 2�� Root Parameter
	// 2�� Root Parameter - ���� : ��Ʈ ���
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	// 2�� Root Parameter - ��� ���� : 32��
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	// 2�� Root Parameter - �������� ��ȣ : 0
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	// 2�� Root Parameter - �������� ���� : 0
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	// 2�� Root Parameter - � Shader���� ��� �����Ѱ�? : Vertex Shader
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// ��Ʈ �ñ״�ó ���̾ƿ�(� ���̴����� ���� �����Ѱ�.)
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// [��Ʈ �ñ״�ó - ����]
	// ��Ʈ �ñ״�ó DESC ����
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	// ��Ʈ �ñ״�ó DESC - ��� ���� �ʱ�ȭ 0 or NULL
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	// ��Ʈ �ñ״�ó DESC - root parameter ���� : 2��
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	// ��Ʈ �ñ״�ó DESC - root parameter ���� �ּ� : pd3dRootParameters
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	// ��Ʈ �ñ״�ó DESC - static sampler ���� (���� ���÷��� ����)
	d3dRootSignatureDesc.NumStaticSamplers = 0; 
	// ��Ʈ �ñ״�ó DESC - static sampler �ּ�
	d3dRootSignatureDesc.pStaticSamplers = NULL; 
	// ��Ʈ �ñ״�ó DESC - ���̾ƿ�(� ���̴����� ���� �����Ѱ�.) : IA�ܰ踦 ���
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	// Blob ����
	ID3DBlob *pd3dSignatureBlob = NULL; 
	ID3DBlob *pd3dErrorBlob = NULL;

	// ������ ��Ʈ �ñ״�ó DESC�� ����, ����, Blob�� �̿��� (�Ҵ� ?)
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	// pd3dDevice�� RootSignature���� : (���� GPU, ����ȭ�� RootSignature�� ������ ������, RootSignature�� ũ��, riid, ������ RootSignature�� �ּ�)
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),(void **)&pd3dGraphicsRootSignature);
	
	// com��ü Release
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	
	// ������ RootSignature�� �ּҸ� ������
	return pd3dGraphicsRootSignature;
}

