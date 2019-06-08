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

//래스터라이저 상태를 설정하기 위한 구조체를 반환한다.
D3D12_RASTERIZER_DESC Shader::CreateRasterizerState() {
	// Rasterizer Desc 생성
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	// Rasterizer Desc - 초기화
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	// Rasterizer Desc - 채우기 모드 : 면 채워서 그리기
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// Rasterizer Desc - 그리지 않을 방향 : 없음
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// Rasterizer Desc - 전면이 반시계 방향인가?
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	// Rasterizer Desc - 깊이값에 더해질 깊이 바이어스
	d3dRasterizerDesc.DepthBias = 0;
	// Rasterizer Desc - 깊이값에 더해질 최대값
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	// Rasterizer Desc - 기울기에 따라 바이어스 조절값
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	// Rasterizer Desc - 깊이 클리핑 유무 : 0 < z < 1 가 아닌 범위의 클리핑 유무
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	// Rasterizer Desc - 멀티 샘플링 유무
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	// Rasterizer Desc - 안티엘리어싱 유무
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	// Rasterizer Desc - UAV 렌더링에 사용할 샘플의 개수
	d3dRasterizerDesc.ForcedSampleCount = 0;
	// Rasterizer Desc - 보수 레스터 라이저 사용 유무
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	
	return d3dRasterizerDesc;
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState() {
	// (ppt에 내용이 없다?)
	// DepthStencil Desc 생성
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	// DepthStencil Desc - 초기화
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	// DepthStencil Desc - 깊이 테스트 사용 여부
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

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC Shader::CreateBlendState() {
	// (ppt에 내용만 있고 설명이 없다?)
	// Blend Desc 생성
	D3D12_BLEND_DESC d3dBlendDesc;
	// Blend Desc - 초기화
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

//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout() {
	
	// Input Layout DESC 생성 : 입력 원소들의 배열
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout DESC - 원소의 배열 주소
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	// Input Layout DESC - 원소의 개수
	d3dInputLayoutDesc.NumElements = 0;
	
	return(d3dInputLayoutDesc);
}

//정점 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);

}

//픽셀 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다.
D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(const wchar_t *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob) {
	
	// 컴파일 선택사항 - 디버그 정보 추가 | 최적화하지 않음
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// Shader소스파일 이름, 쉐이더 매크로, #include의 여부, Shader함수 이름, Shader 버전, 컴파일 선택사항, 이펙트 선택사항, 코드(?)
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);

	// Shader 실행 코드 (코드 내용)
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	// 실행 코드 사이즈
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	// 실행 코드 주소
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	
	return d3dShaderByteCode;
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void Shader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) {
	
	// VertexShader와 PixelShader 내용을 저장할 Blob 할당
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	// Pipeline State Desc 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	// Pipeline State Desc - 모든 변수 초기화 0 or NULL
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	// Pipeline State Desc - Root Signature의 주소
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	// Pipeline State Desc - Vertex Shader 생성해 Blob에 저장
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	// Pipeline State Desc - Pixel Shader 생성해 Blob에 저장
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	// Pipeline State Desc - 나머지 Shader = NULL : DS HS GS

	// Pipeline State Desc - Rasterizer 생성
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	// Pipeline State Desc - Blend 생성
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	// Pipeline State Desc - DepthStencil 생성
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	// Pipeline State Desc - InputLayout 생성 : 입력 조립기에 정점 버퍼 구조 전달
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();

	// (뭐지?)
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// RenderTarget 개수
	d3dPipelineStateDesc.NumRenderTargets = 1;
	// RenderTarget 포맷 : R8G8B8A8
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// DepthSencil 포맷 
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	// (?)
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Graphics Pipeline State 생성 - 생성한 Desc를 Pipeline States배열의 0번째로
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	// com객체 Release
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
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera) {
	OnPrepareRender(pd3dCommandList);
}

DiffusedShader::DiffusedShader() {

}

DiffusedShader::~DiffusedShader() {

}

D3D12_INPUT_LAYOUT_DESC DiffusedShader::CreateInputLayout(){
	UINT nInputElementDescs = 2;
	
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE DiffusedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE DiffusedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void DiffusedShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature){
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	Shader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}
