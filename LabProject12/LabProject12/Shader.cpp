#include "stdafx.h"
#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader() {
	if (m_ppd3dPipelineStates) {
		for (int i = 0; i < m_nPipelineStates; i++)
			if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

//�����Ͷ����� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_RASTERIZER_DESC Shader::CreateRasterizerState() {
	// Rasterizer Desc ����
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	// Rasterizer Desc - �ʱ�ȭ
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	// Rasterizer Desc - ä��� ��� : �� ä���� �׸���
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// Rasterizer Desc - �׸��� ���� ���� : ��
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// Rasterizer Desc - ������ �ݽð� �����ΰ�?
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	// Rasterizer Desc - ���̰��� ������ ���� ���̾
	d3dRasterizerDesc.DepthBias = 0;
	// Rasterizer Desc - ���̰��� ������ �ִ밪
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	// Rasterizer Desc - ���⿡ ���� ���̾ ������
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	// Rasterizer Desc - ���� Ŭ���� ���� : 0 < z < 1 �� �ƴ� ������ Ŭ���� ����
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	// Rasterizer Desc - ��Ƽ ���ø� ����
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	// Rasterizer Desc - ��Ƽ������� ����
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	// Rasterizer Desc - UAV �������� ����� ������ ����
	d3dRasterizerDesc.ForcedSampleCount = 0;
	// Rasterizer Desc - ���� ������ ������ ��� ����
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	
	return d3dRasterizerDesc;
}

//����-���ٽ� �˻縦 ���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState() {
	// (ppt�� ������ ����?)
	// DepthStencil Desc ����
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	// DepthStencil Desc - �ʱ�ȭ
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	// DepthStencil Desc - ���� �׽�Ʈ ��� ����
	d3dDepthStencilDesc.DepthEnable = TRUE;
	// DepthStencil Desc - 
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

//���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_BLEND_DESC Shader::CreateBlendState() {
	// (ppt�� ���븸 �ְ� ������ ����?)
	// Blend Desc ����
	D3D12_BLEND_DESC d3dBlendDesc;
	// Blend Desc - �ʱ�ȭ
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	// Blend Desc - 
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	
	return d3dBlendDesc;
}

//�Է� �����⿡�� ���� ������ ������ �˷��ֱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout() {
	
	// Input Layout DESC ���� : �Է� ���ҵ��� �迭
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout DESC - ������ �迭 �ּ�
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	// Input Layout DESC - ������ ����
	d3dInputLayoutDesc.NumElements = 0;
	
	return(d3dInputLayoutDesc);
}

//���� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);

}

//�ȼ� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

//���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ�Ѵ�.
D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(const wchar_t *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob) {
	
	// ������ ���û��� - ����� ���� �߰� | ����ȭ���� ����
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// Shader�ҽ����� �̸�, ���̴� ��ũ��, #include�� ����, Shader�Լ� �̸�, Shader ����, ������ ���û���, ����Ʈ ���û���, �ڵ�(?)
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);

	// Shader ���� �ڵ� (�ڵ� ����)
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	// ���� �ڵ� ������
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	// ���� �ڵ� �ּ�
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	
	return d3dShaderByteCode;
}

//�׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�.
void Shader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) {
	
	// VertexShader�� PixelShader ������ ������ Blob �Ҵ�
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	// Pipeline State Desc ����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	// Pipeline State Desc - ��� ���� �ʱ�ȭ 0 or NULL
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	// Pipeline State Desc - Root Signature�� �ּ�
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	// Pipeline State Desc - Vertex Shader ������ Blob�� ����
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	// Pipeline State Desc - Pixel Shader ������ Blob�� ����
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	// Pipeline State Desc - ������ Shader = NULL : DS HS GS

	// Pipeline State Desc - Rasterizer ����
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	// Pipeline State Desc - Blend ����
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	// Pipeline State Desc - DepthStencil ����
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	// Pipeline State Desc - InputLayout ���� : �Է� �����⿡ ���� ���� ���� ����
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();

	// (����?)
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// RenderTarget ����
	d3dPipelineStateDesc.NumRenderTargets = 1;
	// RenderTarget ���� : R8G8B8A8
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// DepthSencil ���� 
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	// (?)
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Graphics Pipeline State ���� - ������ Desc�� Pipeline States�迭�� 0��°��
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	// com��ü Release
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) {
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList) {
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera) {
	OnPrepareRender(pd3dCommandList);
}

PlayerShader::PlayerShader() {

}

PlayerShader::~PlayerShader() {

}

D3D12_INPUT_LAYOUT_DESC PlayerShader::CreateInputLayout(){
	
	// 2������ �Է� ���۸� ����ڴ�.
	// ������ ��ġ, ��
	UINT nInputElementDescs = 2;
	
	// ���ҹ迭�� 2�� ����
	// pd3dInputElementDescs[0] ���� ��ġ, pd3dInputElementDescs[1] ���� ���� ����
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	// Input Layout Desc ����
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout Desc - �ּ�
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	// Input Layout Desc - ���� ������ ����
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE PlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE PlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void PlayerShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature){
	// ���������� 1���� �����ϰڴ�
	m_nPipelineStates = 1;
	// ���������� �Ѱ��� �Ҵ�
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

ObjectsShader::ObjectsShader() {
}

ObjectsShader::~ObjectsShader() {
}

D3D12_INPUT_LAYOUT_DESC ObjectsShader::CreateInputLayout() {

	// 2������ �Է� ���۸� ����ڴ�.
	// ������ ��ġ, ��
	UINT nInputElementDescs = 2;

	// ���ҹ迭�� 2�� ����
	// pd3dInputElementDescs[0] ���� ��ġ, pd3dInputElementDescs[1] ���� ���� ����
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	// Input Layout Desc ����
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout Desc - �ּ�
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	// Input Layout Desc - ���� ������ ����
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE ObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE ObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void ObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) {
	// 1���� ������ ������ �����ϰڴ�.
	m_nPipelineStates = 1;
	// ���������� ����
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void ObjectsShader::ReleaseUploadBuffers(){
	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void ObjectsShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, Camera * pCamera){
	Shader::Render(pd3dCommandList, pCamera);
	
	for (int j = 0; j < m_nObjects; j++) {
		if (m_ppObjects[j]) {
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void ObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {
	
}

void ObjectsShader::AnimateObjects(float fTimeElapsed){
	for (int j = 0; j < m_nObjects; j++) {
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void ObjectsShader::ReleaseObjects(){
	if (m_ppObjects) { 
		for (int j = 0; j < m_nObjects; j++) {
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}

}

InstancingShader::InstancingShader() {

 }

InstancingShader::~InstancingShader() {

}

D3D12_INPUT_LAYOUT_DESC InstancingShader::CreateInputLayout(){

	// ������ ������ 7��
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	
	// (�� ��ǥ��) ���� ������ ���� �Է� �����̴�.
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	// (���� ��ǥ��) �ν��Ͻ� ������ ���� �Է� �����̴�.
	pd3dInputElementDescs[2] = { "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[3] = { "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[4] = { "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[5] = { "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	// ��ü ����
	pd3dInputElementDescs[6] = { "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);

}

D3D12_SHADER_BYTECODE InstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE InstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1", ppd3dShaderBlob));
}

void InstancingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) {
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	
	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void InstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {
	// �ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� �����Ѵ�.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	
	// ���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
	
	// ���� ���ۿ� ���� �並 �����Ѵ�.
	m_d3dInstancingBufferView.BufferLocation = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	m_d3dInstancingBufferView.StrideInBytes = sizeof(VS_VB_INSTANCE);
	m_d3dInstancingBufferView.SizeInBytes = sizeof(VS_VB_INSTANCE) * m_nObjects;
}

void InstancingShader::ReleaseShaderVariables() {
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}

void InstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {
	//�ν��Ͻ� ����(��ü�� ���� ��ȯ ��İ� ����)�� ���� ���ۿ� �����Ѵ�.
	for (int j = 0; j < m_nObjects; j++) {
		// ��ü�� ���� ���� (Ȧ���̸� ����, ¦���̸� �Ķ�)
		m_pcbMappedGameObjects[j].m_xmcColor = (j % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}

void InstancingShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {
	// x��, y��, z���� ũ��
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	
	// ��ü ��ü�� ��
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	// ��ü ��ü�� �� ��ŭ �迭 ����
	m_ppObjects = new GameObject*[m_nObjects];
	// ��ü�� ũ�� ����
	float fxPitch = 12.0f * 2.5f; float fyPitch = 12.0f * 2.5f; float fzPitch = 12.0f * 2.5f;

	// ȸ���ϴ� ��ü�� ����ڴ�.
	RotatingObject *pRotatingObject = NULL;

	// -x ~ +x
	for (int x = -xObjects; x <= xObjects; x++) {
		// -y ~ +y
		for (int y = -yObjects; y <= yObjects; y++) {
			// -z ~ +z
			for (int z = -zObjects; z <= zObjects; z++) {
				// ȸ���ϴ� ��ü ����
				pRotatingObject = new RotatingObject();
				// ��ü ��ġ Set
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				// ��ü ȸ�� ���� y������ Set
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				// ��ü ȸ�� �ӵ� Set
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				// ��ü�� �迭��
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	// ȸ�� ��ü�� �迭�� 0��°�� Mesh�� ����
	m_ppObjects[0]->SetMesh(pCubeMesh);

	//�ν��Ͻ��� ���� ���� ���ۿ� �並 �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void InstancingShader::ReleaseObjects(){

}

void InstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera) {
	Shader::Render(pd3dCommandList, pCamera);
	
	//��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);
	
	//�ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects, m_d3dInstancingBufferView);
}
