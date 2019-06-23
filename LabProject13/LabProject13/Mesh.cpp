#include "stdafx.h"
#include "Mesh.h"

//정점을 표현하기 위한 클래스를 선언.
 class Vertex { 
 protected: 
	 //정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).
	 XMFLOAT3 m_xmf3Position;

public:
	Vertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	Vertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~Vertex() { } 
 };

class DiffusedVertex : public Vertex {
protected: 
	//정점의 색상이다.
	XMFLOAT4 m_xmf4Diffuse;
public: 
	DiffusedVertex() { 
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	DiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z);
		m_xmf4Diffuse = xmf4Diffuse; 
	}

	DiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position;
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~DiffusedVertex() { }
};

Mesh::Mesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {

}

Mesh::~Mesh() {
	// 버텍스 버퍼 & 업로드 버퍼
	if (m_pd3dVertexBuffer) 		m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) 	m_pd3dVertexUploadBuffer->Release();
	
	// 인덱스 버퍼 & 업로드 버퍼
	if (m_pd3dIndexBuffer)			m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer)	m_pd3dIndexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers() {
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	// 인덱스 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	//메쉬의 프리미티브 유형을 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//메쉬의 정점 버퍼 뷰를 설정한다.
	// 정점버퍼의 시작 입력 슬롯(디바이스), 정점 버퍼의 개수, 정점버퍼 뷰 주소
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	
	//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링
	if (m_pd3dIndexBuffer) {
		// 파이프라인에 하나의 인덱스 버퍼 연결 가능
		// 인덱스 버퍼 주소
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);

		// 인덱스 개수, 객체 수, 시작 인덱스 위치, 각 정점 인덱스에 더해질 값, 객체 인덱스에 더해질 값
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	} 

	//메쉬의 정점 버퍼 뷰를 렌더링 (파이프라인(입력 조립기)을 작동하게 한다).
	// 정점 개수, 객체 수, 시작 정점 위치, 객체 인덱스에 더해질 값
	else	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

void Mesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nInstances) {
	// 입력조립기에 정점버퍼를 Set한다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	// 입력조립기에 프리미티브 토폴로지를 Set한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	// 인덱스 버퍼가 있다면 인덱스버퍼를 Set하고, 객체 수만큼 그린다.
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}

	// 인덱스 버퍼가 없다면 정점버퍼로 그린다.
	else {
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}

}

TriangleMesh::TriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : Mesh(pd3dDevice, pd3dCommandList) {
	// 삼각형 메쉬를 정의한다.
	// 메쉬의 정점의 개수
	m_nVertices = 3;
	// 메쉬의 색상, 위치를 가진 정점을 표현하는 class의 크기 - 정점 버퍼 원소 하나의 크기
	m_nStride = sizeof(DiffusedVertex);
	// Primitive Topology
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다.
	//RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현한다.
	//각 파라메터는 0.0~1.0 사이의 실수값을 가진다.

	// 색, 위치 정보를 가진 정점 3개 생성
	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	// 삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	// 리소스와 내부적 힙을 동시에 생성
	// 디바이스, 커맨드 리스트, 정점데이터 주소, 정점 버퍼 전체의 크기, 힙의 유형, 리소스 버퍼의 상태, 업로드 버퍼의 주소
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 정점 버퍼 뷰를 생성한다.
	// 정점 버퍼 주소
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	// 정점 버퍼 전체의 크기
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	// 정점 버퍼 각 원소의 크기, 하나의 정점 크기
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
}

CubeMeshDiffused::CubeMeshDiffused(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	float fWidth, float fHeight, float fDepth){

	// 객체에 필요한 정점 수 8개 (직육면체)
	m_nVertices = 8;
	m_nStride = sizeof(DiffusedVertex);
	
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이 
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	
	// [1] 정점 버퍼
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
	DiffusedVertex pVertices[8];
	pVertices[0] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	// 삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	// 리소스와 내부적 힙을 동시에 생성
	// 디바이스, 커맨드 리스트, 정점데이터 주소, 정점 버퍼 전체의 크기, 힙의 유형(기본), 리소스 버퍼의 상태(상수버퍼), 업로드 버퍼의 주소
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	//정점 버퍼 뷰를 생성한다.
	// 정점 버퍼 주소
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	// 정점 버퍼 각 원소의 크기, 하나의 정점 크기
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	// 정점 버퍼 전체의 크기
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	// [2] 인덱스 버퍼
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다.
	삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다.
	즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	
	// 8개의 정점으로 36개의 삼각형 꼭지점 정하기
	// 인덱스 개수
	m_nIndices = 36;
	UINT pnIndices[36];

	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형   
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형   
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형   
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형 
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형   
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형   
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//인덱스 버퍼를 생성
	// 디바이스, 커맨드 리스트, 정점데이터 주소, 정점 버퍼 전체의 크기, 힙의 유형(기본), 리소스 버퍼의 상태(인덱스 버퍼), 업로드 버퍼의 주소
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	//인덱스 버퍼 뷰를 생성
	// 인덱스 버퍼 주소
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	// 인덱스 버퍼 포멧
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// 인덱스 버퍼 전체 크기 : UINT * 인덱스의 개수
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices; 
}

CubeMeshDiffused::~CubeMeshDiffused(){

}

AirplaneMeshDiffused::AirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : Mesh(pd3dDevice, pd3dCommandList) {
	
	m_nVertices = 24 * 3; m_nStride = sizeof(DiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다.
	DiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	XMFLOAT4 randomcol = RANDOM_COLOR;

	//비행기 메쉬의 위쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	
	//비행기 메쉬의 아래쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	
	//비행기 메쉬의 오른쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	
	//비행기 메쉬의 뒤쪽/오른쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	
	//비행기 메쉬의 왼쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	
	//비행기 메쉬의 뒤쪽/왼쪽 면
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, randomcol)); randomcol = RANDOM_COLOR;
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, randomcol));
	
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

AirplaneMeshDiffused::~AirplaneMeshDiffused() {
}
