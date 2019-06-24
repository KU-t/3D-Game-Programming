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

protected:
	// 인덱스 버퍼 인터페이스 포인터
	ID3D12Resource *m_pd3dIndexBuffer = NULL;
	// 인덱스 버퍼 업로드 버퍼 인터페이스 포인터
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL;

	// 인덱스 버퍼 뷰
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	// 인덱스 버퍼에 포함되는 인덱스의 개수이다.
	UINT m_nIndices = 0;
	// 인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다.
	UINT m_nStartIndex = 0;
	// 인덱스 버퍼의 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex = 0;

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

class AirplaneMeshDiffused : public Mesh {
public:
	AirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~AirplaneMeshDiffused();
};

class HeightMapImage {
private:
	//높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	BYTE *m_pHeightMapPixels;
	
	//높이 맵 이미지의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	
	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;

public:
	HeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~HeightMapImage(void);

	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.
	float GetHeight(float x, float z);

	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class HeightMapGridMesh : public Mesh {
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다.
	int m_nWidth;
	int m_nLength;
	
	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다.
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다.
	즉, 실제 격자의 x-축 방향의 간격은 1이 아니 라 스케일 벡터의 x-좌표가 된다.
	이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할 수 있다.*/
	XMFLOAT3 m_xmf3Scale;

public:
	HeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		int xStart, int zStart, int nWidth, int nLength,
		XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void *pContext = NULL);
	virtual ~HeightMapGridMesh();
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength);}

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다.
	virtual float OnGetHeight(int x, int z, void *pContext);

	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다.
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};
