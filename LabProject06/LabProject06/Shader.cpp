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
	// Rasterizer Desc - 그리지 않을 방향 : 뒤
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
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
	// 2가지의 입력 데이터 : 위치, 색원소 data를 저장
	UINT nInputElementDescs = 2;

	// 2가지의 입력 데이터 배열 생성
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	// 정점은 위치 벡터(POSITION)와 색상(COLOR)을 가진다.
	// 시멘틱 문자열, 시멘틱 인덱스, 원소의 데이터형, 입력슬롯(최대 16개), 정렬 오프셋, 데이터 유형(정점, 객체), 몇개의 객체에 반복되는가(0 == 정점 데이터)
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	// Input Layout DESC 생성 : 입력 원소들의 배열
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	// Input Layout DESC - 원소의 배열 주소
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	// Input Layout DESC - 원소의 개수
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

//정점 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	return CompileShaderFromFile(L"Shaders.hlsl", "VSMain", "vs_5_1", ppd3dShaderBlob);
}

//픽셀 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	return CompileShaderFromFile((L"Shaders.hlsl"), "PSMain", "ps_5_1", ppd3dShaderBlob);
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

	// Shader 실행 코드
	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	// 실행 코드 사이즈
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	// 실행 코드 주소
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void Shader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dRootSignature) {

	// [ GraphicsPipelineState 객체 생성 ]
	// 1개의 GraphicsPipeline를 만들겠다
	m_nPipelineStates = 1;

	// 1개의 PipelineState를 할당
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	// VertexShader와 pixelShader 내용을 저장할 Blob 할당
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	// Pipeline State Desc 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	// Pipeline State Desc - 모든 변수 초기화 0 or NULL
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	// Pipeline State Desc - Root Signature의 주소
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
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
	// RenderTarget 포맷 R8G8B8A8
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// DepthSencil 포맷 
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;

	// Graphics Pipeline State 생성 - 생성한 Desc를 Pipeline States배열의 0번째로
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	// (의미가 ?)
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//Shader 객체가 포함하는 게임 객체들을 생성한다.
void Shader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext) {
	// 삼각형 메쉬를 생성한다.
	TriangleMesh *pTriangleMesh = new TriangleMesh(pd3dDevice, pd3dCommandList);

	// Shader에서의 객체의 수 : 1
	m_nObjects = 1;
	// Shader에서의 게임 객체 리스트에 1개의 게임 객체를 생성할 메모리 할당
	m_ppObjects = new GameObject*[m_nObjects];

	// 리스트 0번자리에 할당하고 게임객체 생성
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
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	OnPrepareRender(pd3dCommandList);
	for (int j = 0; j < m_nObjects; j++) {
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList);
	}
}
