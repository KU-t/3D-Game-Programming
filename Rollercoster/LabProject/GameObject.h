#pragma once

#include "Mesh.h"
#include "Camera.h"

class CGameObject{
public:
	CGameObject();
	CGameObject(CMesh *pMesh);
	virtual ~CGameObject();

public:
	bool						m_bActive = true;
	

	CMesh						*m_pMesh = NULL;
    XMFLOAT4X4					m_xmf4x4World; 

	XMFLOAT3					m_xmf3Look;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Right;

	DWORD						m_dwColor;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	void Rotate(XMFLOAT3& rotate);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	void Move(XMFLOAT3& vDirection, float fDistance);

	void SetLook(XMFLOAT3& look);
	void SetUp(XMFLOAT3& up);
	void SetRight(XMFLOAT3& right);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Animate(float fElapsedTime);
	virtual void OnPreRender() { }
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

