#pragma once

#include "Camera.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertex{
public:
	CVertex() { }
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }

	XMFLOAT3					m_xmf3Position;
};

class CPolygon{
public:
	CPolygon(int nVertices);
	virtual ~CPolygon();

	int							m_nVertices;
	CVertex						*m_pVertices;

	void SetVertex(int nIndex, CVertex& vertex);
};

class CMesh{
public:
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int							m_nReferences;

	int							m_nPolygons;
	CPolygon					**m_ppPolygons;

public:
	void AddRef() { m_nReferences++; }
	void Release() { 
		m_nReferences--; 
		if (m_nReferences <= 0) 
			delete this;
	}

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);
	virtual void Render(HDC hDCFrameBuffer, XMFLOAT4X4& xmf4x4World, CCamera *pCamera);

};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CCubeMesh();
};

class CAirplaneMesh : public CMesh {
public:
	CAirplaneMesh(float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~CAirplaneMesh();
};

class CRailMesh : public CMesh {
public:
	CRailMesh();
	virtual ~CRailMesh();
};

class CTerrainMesh : public CMesh{
public:
	CTerrainMesh(float fWidth = 100.0f, float fHeight = 4.0f, float fDepth = 100.0f, int nSubRects = 20);
	virtual ~CTerrainMesh();
};