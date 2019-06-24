#include "stdafx.h"
#include "Mesh.h"

//������ ǥ���ϱ� ���� Ŭ������ ����.
 class Vertex { 
 protected: 
	 //������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�).
	 XMFLOAT3 m_xmf3Position;

public:
	Vertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	Vertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~Vertex() { } 
 };

class DiffusedVertex : public Vertex {
protected: 
	//������ �����̴�.
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
	// ���ؽ� ���� & ���ε� ����
	if (m_pd3dVertexBuffer) 		m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) 	m_pd3dVertexUploadBuffer->Release();
	
	// �ε��� ���� & ���ε� ����
	if (m_pd3dIndexBuffer)			m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer)	m_pd3dIndexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers() {
	// ���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	// �ε��� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	//�޽��� ������Ƽ�� ������ �����Ѵ�.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//�޽��� ���� ���� �並 �����Ѵ�.
	// ���������� ���� �Է� ����(����̽�), ���� ������ ����, �������� �� �ּ�
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	
	//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� ������
	if (m_pd3dIndexBuffer) {
		// ���������ο� �ϳ��� �ε��� ���� ���� ����
		// �ε��� ���� �ּ�
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);

		// �ε��� ����, ��ü ��, ���� �ε��� ��ġ, �� ���� �ε����� ������ ��, ��ü �ε����� ������ ��
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	} 

	//�޽��� ���� ���� �並 ������ (����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
	// ���� ����, ��ü ��, ���� ���� ��ġ, ��ü �ε����� ������ ��
	else	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

TriangleMesh::TriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : Mesh(pd3dDevice, pd3dCommandList) {
	// �ﰢ�� �޽��� �����Ѵ�.
	// �޽��� ������ ����
	m_nVertices = 3;
	// �޽��� ����, ��ġ�� ���� ������ ǥ���ϴ� class�� ũ�� - ���� ���� ���� �ϳ��� ũ��
	m_nStride = sizeof(DiffusedVertex);
	// Primitive Topology
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� �����Ѵ�.
	//RGBA(Red, Green, Blue, Alpha) 4���� �Ķ���͸� ����Ͽ� ������ ǥ���Ѵ�.
	//�� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.

	// ��, ��ġ ������ ���� ���� 3�� ����
	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	// �ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	// ���ҽ��� ������ ���� ���ÿ� ����
	// ����̽�, Ŀ�ǵ� ����Ʈ, ���������� �ּ�, ���� ���� ��ü�� ũ��, ���� ����, ���ҽ� ������ ����, ���ε� ������ �ּ�
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���� ���� �並 �����Ѵ�.
	// ���� ���� �ּ�
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	// ���� ���� ��ü�� ũ��
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	// ���� ���� �� ������ ũ��, �ϳ��� ���� ũ��
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
}

CubeMeshDiffused::CubeMeshDiffused(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	float fWidth, float fHeight, float fDepth){

	// ��ü�� �ʿ��� ���� �� 8�� (������ü)
	m_nVertices = 8;
	m_nStride = sizeof(DiffusedVertex);
	
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	//fWidth: ������ü ����(x-��) ����, fHeight: ������ü ����(y-��) ����, fDepth: ������ü ����(z-��) ���� 
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	
	// [1] ���� ����
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.
	DiffusedVertex pVertices[8];
	pVertices[0] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	// �ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	// ���ҽ��� ������ ���� ���ÿ� ����
	// ����̽�, Ŀ�ǵ� ����Ʈ, ���������� �ּ�, ���� ���� ��ü�� ũ��, ���� ����(�⺻), ���ҽ� ������ ����(�������), ���ε� ������ �ּ�
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	//���� ���� �並 �����Ѵ�.
	// ���� ���� �ּ�
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	// ���� ���� �� ������ ũ��, �ϳ��� ���� ũ��
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	// ���� ���� ��ü�� ũ��
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	// [2] �ε��� ����
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�.
	�ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�.
	��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	
	// 8���� �������� 36���� �ﰢ�� ������ ���ϱ�
	// �ε��� ����
	m_nIndices = 36;
	UINT pnIndices[36];

	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��   
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��   
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��   
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//�ε��� ���۸� ����
	// ����̽�, Ŀ�ǵ� ����Ʈ, ���������� �ּ�, ���� ���� ��ü�� ũ��, ���� ����(�⺻), ���ҽ� ������ ����(�ε��� ����), ���ε� ������ �ּ�
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	
	//�ε��� ���� �並 ����
	// �ε��� ���� �ּ�
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	// �ε��� ���� ����
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// �ε��� ���� ��ü ũ�� : UINT * �ε����� ����
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

	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�.
	DiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	XMFLOAT4 randomcol = RANDOM_COLOR;

	//����� �޽��� ���� ��
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
	
	//����� �޽��� �Ʒ��� ��
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
	
	//����� �޽��� ������ ��
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
	
	//����� �޽��� ����/������ ��
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
	
	//����� �޽��� ���� ��
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
	
	//����� �޽��� ����/���� ��
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

HeightMapImage::HeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale) {
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	// ���� �ȼ� ���̸�ŭ �迭 ����
	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	//������ ���� �д´�. ���� �� �̹����� ���� ����� ���� RAW �̹����̴�.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead; ::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	// ���� ���Ͽ� �����Ѵ�. ��, ���δ� �״�� ���δ� �ݴ�� �ִ´�.
	// ���������� (0.0)�̴ϱ�!
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++) {
		for (int x = 0; x < m_nWidth; x++) {
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = 100;
			int a = 0;
		}
	}

	// ��������� ���� �迭�� ����
	if (pHeightMapPixels) 
		delete[] pHeightMapPixels;
}

HeightMapImage::~HeightMapImage(void) {
	if (m_pHeightMapPixels) 
		delete[] m_pHeightMapPixels;

	m_pHeightMapPixels = NULL;
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float HeightMapImage::GetHeight(float fx, float fz) {
	// ������ ��� ������ (0,0)�� ����
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength))
		return(0.0f);

	// ���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�.
	// ������
	int x = (int)fx;
	int z = (int)fz;
	// �Ҽ���
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	// �� ������ ���� ���
	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER 
	//z-��ǥ�� 1, 3, 5, ...�� ��� �ε����� �����ʿ��� �������� �����ȴ�.
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft) {
		/*������ �ﰢ������ �����ʿ��� ���� �������� �����Ǵ� ����̴�.
		���� �׸��� �������� (fzPercent < fxPercent) �� ����̴�.
		�� ��� TopLeft�� �ȼ� ���� (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))�� �ٻ��Ѵ�.
		���� �׸��� ������ (fzPercent �� fxPercent)�� ����̴�.
		�� ��� BottomRight�� �ȼ� ���� (fBottomRight = fBottomLeft + (fTopRight - fTopLeft))�� �ٻ��Ѵ�.*/

		if (fzPercent >= fxPercent) fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else {
		/*������ �ﰢ������ ���ʿ��� ������ �������� �����Ǵ� ����̴�.
		���� �׸��� ������ (fzPercent < (1.0f - fxPercent))�� ����̴�.
		�� ��� TopRight�� �ȼ� ���� (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))�� �ٻ��Ѵ�.
		���� �׸��� �������� (fzPercent �� (1.0f - fxPercent))�� ����̴�.
		�� ��� BottomLeft�� �ȼ� ���� (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))�� �ٻ��Ѵ�.*/
		
		if (fzPercent < (1.0f - fxPercent)) fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif 

	//�簢���� �� ���� �����Ͽ� ����(�ȼ� ��)�� ����Ѵ�.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
		
	return(fHeight);
}

XMFLOAT3 HeightMapImage::GetHeightMapNormal(int x, int z) {
	
	//x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���� ���ʹ� y-�� ���� �����̴�.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/*���� �ʿ��� (x, z) ��ǥ�� �ȼ� ���� ������ �� ���� �� (x+1, z), (z, z+1)�� ���� �ȼ� ���� ����Ͽ� ���� ���͸� ����Ѵ�.*/ 
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	//(x, z), (x+1, z), (z, z+1)�� �ȼ����� ������ ���̸� ���Ѵ�.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	
	//xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�.
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	
	//xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	//���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	
	return(xmf3Normal);
}

HeightMapGridMesh::HeightMapGridMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void * pContext) {
	//������ ����(����)�� ������ (nWidth * nLength)�̴�.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(DiffusedVertex);

	//���ڴ� �ﰢ�� ��Ʈ������ �����Ѵ�.
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth; m_nLength = nLength; m_xmf3Scale = xmf3Scale;
	DiffusedVertex *pVertices = new DiffusedVertex[m_nVertices];

	/*xStart�� zStart�� ������ ���� ��ġ(x-��ǥ�� z-��ǥ)�� ��Ÿ����.
	Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ���� �� �䰡 �ֱ� ������
	��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.*/
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++) {
		for (int x = xStart; x < (xStart + nWidth); x++, i++) {
			//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			XMFLOAT3 xmf3Position = XMFLOAT3((x*m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z*m_xmf3Scale.z));

			// ??????????????????????????????????????????????????????????
			XMFLOAT4 xmOnGetColor = OnGetColor(x, z, pContext);

			XMFLOAT4 xmf3Color = Vector4::Add(xmOnGetColor, xmf4Color);
			pVertices[i] = DiffusedVertex(xmf3Position, xmf3Color);

			if (fHeight < fMinHeight)
				fMinHeight = fHeight;
			if (fHeight > fMaxHeight)
				fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer =
		::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	
	delete[] pVertices;

	/*�̷��� �ε����� �����ϸ� �ε��� ���۴� ((nWidth*2)*(nLength-1))+((nLength-1)-1)���� �ε����� ���´�.
	�簢 �� ���� ������ (nLength-1)�̰� �� �ٿ��� (nWidth*2)���� �ε����� ���´�.
	�׸��� ���� �ٲ� ������ �ε����� �� �� �߰��ϹǷ� (nLength-1)-1���� �ε����� �߰��� �ʿ��ϴ�.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	UINT *pnIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < nLength - 1; z++) {
		if ((z % 2) == 0) {

			//Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ �����̴�.
			for (int x = 0; x < nWidth; x++) {

				//ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰��Ѵ�.
				if ((x == 0) && (z > 0))
					pnIndices[j++] = (UINT)(x + (z * nWidth));

				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				pnIndices[j++] = (UINT)(x + (z * nWidth)); pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}

		else {
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�.
			for (int x = nWidth - 1; x >= 0; x--) {

				//���� �ٲ� ������(x == (nWidth-1)) ù ��° �ε����� �߰��Ѵ�.
				if (x == (nWidth - 1))
					pnIndices[j++] = (UINT)(x + (z * nWidth));

				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = 
		::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

HeightMapGridMesh::~HeightMapGridMesh() {

}

float HeightMapGridMesh::OnGetHeight(int x, int z, void * pContext) {
	HeightMapImage *pHeightMapImage = (HeightMapImage *)pContext;
	BYTE *pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();

	float fHeight = pHeightMapPixels[x + (z*nWidth)] * xmf3Scale.y;

	return(fHeight);
}

XMFLOAT4 HeightMapGridMesh::OnGetColor(int x, int z, void * pContext) {
	//������ ���� ����(�������� ��������� ����)�̴�.
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	HeightMapImage *pHeightMapImage = (HeightMapImage *)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	
	//������ ����(����, ���)�̴�.
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*���� (x, z)���� ������ �ݻ�Ǵ� ��(����)��
	���� (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)��
	������ 3�� �� ���� (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ�
	������ ���� ������ ������ ����Ͽ� ���Ѵ�.
	���� (x, z)�� �� ���� ���� ����(����)�� �ݻ�Ǵ� ��(����)�� ���� ���̴�.*/ 

	// ??????????????????????????????????????????????????????????????
	XMFLOAT3 xmGetHeightMapNormal1 = pHeightMapImage->GetHeightMapNormal(x, z);
	XMFLOAT3 xmGetHeightMapNormal2 = pHeightMapImage->GetHeightMapNormal(x + 1, z);
	XMFLOAT3 xmGetHeightMapNormal3 = pHeightMapImage->GetHeightMapNormal(x + 1, z + 1);
	XMFLOAT3 xmGetHeightMapNormal4 = pHeightMapImage->GetHeightMapNormal(x, z + 1);

	float fScale = Vector3::DotProduct(xmGetHeightMapNormal1, xmf3LightDirection);
	fScale += Vector3::DotProduct(xmGetHeightMapNormal2, xmf3LightDirection);
	fScale += Vector3::DotProduct(xmGetHeightMapNormal3, xmf3LightDirection);
	fScale += Vector3::DotProduct(xmGetHeightMapNormal4, xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f) 
		fScale = 1.0f;
	if (fScale < 0.25f) 
		fScale = 0.25f;
	
	//fScale�� ���� ����(���)�� �ݻ�Ǵ� �����̴�.
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color); 
}
