#pragma once

class Mesh {
public:  
	Mesh() {}
	Mesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~Mesh();

private: 
	int m_nReferences = 0;

public: 
	void AddRef() { m_nReferences++; }
	void Release() {
		if (--m_nReferences <= 0)
			delete this;
	}
	void ReleaseUploadBuffers();

protected: 
	// 리소스 버퍼
	ID3D12Resource *m_pd3dVertexBuffer = NULL;
	// 업로드 리소스 버퍼
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;
	// 정점버퍼 뷰
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	// Primitive Topology
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
};

class TriangleMesh : public Mesh {
public:    
	TriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual ~TriangleMesh() { } 
};

class CubeMeshDiffused : public Mesh {
public: 
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다.
	CubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CubeMeshDiffused();
};