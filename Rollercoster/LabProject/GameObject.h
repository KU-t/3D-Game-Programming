#pragma once

#include "Mesh.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject();
	CGameObject(CMesh *pMesh);
	virtual ~CGameObject();

public:
	bool						m_bActive = true;
	
	CMesh						*m_pMesh = NULL;
    XMFLOAT4X4					m_xmf4x4World; 

	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	BoundingOrientedBox			m_xmOOBB;

	CGameObject					*m_pObjectCollided = NULL;

	DWORD						m_dwColor;

	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed = 0.0f;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance=1.0f);
	void MoveUp(float fDistance=1.0f);
	void MoveForward(float fDistance=1.0f);
	void Move(XMFLOAT3& vDirection, float fDistance);

	void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	void Rotate(XMFLOAT3& rotate);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

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

	//test
	void show();
};

