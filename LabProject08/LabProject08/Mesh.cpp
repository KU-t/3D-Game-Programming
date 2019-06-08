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
	if (m_pd3dVertexBuffer) 
		m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) 
		m_pd3dVertexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers() {
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	//메쉬의 프리미티브 유형을 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//메쉬의 정점 버퍼 뷰를 설정한다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	//메쉬의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력 조립기)을 작동하게 한다).
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
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

	// 삼각형 2개로 된 6개의 면 == 3 x 2 x 6
	m_nVertices = 36;
	m_nStride = sizeof(DiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이 
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	DiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR); 
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR); 
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR); 
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR); 
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

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
}

CubeMeshDiffused::~CubeMeshDiffused(){

}
