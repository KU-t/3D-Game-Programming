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
	// 2������ �Է� ������ : ��ġ, ������ data�� ����
	UINT nInputElementDescs = 2;

	// 2������ �Է� ������ �迭 ����
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	// ������ ��ġ ����(POSITION)�� ����(COLOR)�� ������.
	// �ø�ƽ ���ڿ�, �ø�ƽ �ε���, ������ ��������, �Է½���(�ִ� 16��), ���� ������, ������ ����(����, ��ü), ��� ��ü�� �ݺ��Ǵ°�(0 == ���� ������)
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	// Input Layout DESC ���� : �Է� ���ҵ��� �迭
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout DESC - ������ �迭 �ּ�
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	// Input Layout DESC - ������ ����
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

//���� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return CompileShaderFromFile(L"Shaders.hlsl", "VSMain", "vs_5_1", ppd3dShaderBlob);
}

//�ȼ� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return CompileShaderFromFile((L"Shaders.hlsl"), "PSMain", "ps_5_1", ppd3dShaderBlob);
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

	// Shader ���� �ڵ�
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	// ���� �ڵ� ������
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	// ���� �ڵ� �ּ�
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

//�׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�.
void Shader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dRootSignature) {

	// [ GraphicsPipelineState ��ü ���� ]
	// 1���� GraphicsPipeline�� ����ڴ�
	m_nPipelineStates = 1;

	// 1���� PipelineState�� �Ҵ�
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	// VertexShader�� pixelShader ������ ������ Blob �Ҵ�
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	// Pipeline State Desc ����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	// Pipeline State Desc - ��� ���� �ʱ�ȭ 0 or NULL
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	// Pipeline State Desc - Root Signature�� �ּ�
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
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
	// RenderTarget ���� R8G8B8A8
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// DepthSencil ���� 
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;

	// Graphics Pipeline State ���� - ������ Desc�� Pipeline States�迭�� 0��°��
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	// (�ǹ̰� ?)
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//Shader ��ü�� �����ϴ� ���� ��ü���� �����Ѵ�.
void Shader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext) {
	// �ﰢ�� �޽��� �����Ѵ�.
	TriangleMesh *pTriangleMesh = new TriangleMesh(pd3dDevice, pd3dCommandList);

	// Shader������ ��ü�� �� : 1
	m_nObjects = 1;
	// Shader������ ���� ��ü ����Ʈ�� 1���� ���� ��ü�� ������ �޸� �Ҵ�
	m_ppObjects = new GameObject*[m_nObjects];

	// ����Ʈ 0���ڸ��� �Ҵ��ϰ� ���Ӱ�ü ����
	m_ppObjects[0] = new GameObject();
	m_ppObjects[0]->SetMesh(pTriangleMesh);
}

void Shader::ReleaseObjects() {
	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void Shader::AnimateObjects(float fTimeElapsed) {
	for (int j = 0; j < m_nObjects; j++) {
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void Shader::ReleaseUploadBuffers() {
	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList) {
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	OnPrepareRender(pd3dCommandList);
	for (int j = 0; j < m_nObjects; j++) {
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList);
	}
}
