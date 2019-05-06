#include "StdAfx.h"
#include "GameObject.h"

#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject() 
{ 
	m_pMesh = NULL; 
	m_xmf4x4World = Matrix4x4::Identity();

	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_dwColor = RGB(0, 0, 0);

	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	m_pMesh = pMesh; 
}

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z) 
{
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) 
{ 
	m_xmf4x4World._41 = xmf3Position.x; 
	m_xmf4x4World._42 = xmf3Position.y; 
	m_xmf4x4World._43 = xmf3Position.z; 
}

void CGameObject::SetLook(XMFLOAT3& look) {
	m_xmf4x4World._31 = look.x;
	m_xmf4x4World._32 = look.y;
	m_xmf4x4World._33 = look.z;
}

void CGameObject::SetUp(XMFLOAT3& up){
	m_xmf4x4World._21 = up.x;
	m_xmf4x4World._22 = up.y;
	m_xmf4x4World._23 = up.z;
}

void CGameObject::SetRight(XMFLOAT3& right) {
	m_xmf4x4World._11 = right.x;
	m_xmf4x4World._12 = right.y;
	m_xmf4x4World._13 = right.z;
}

XMFLOAT3 CGameObject::GetPosition() 
{ 
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}

XMFLOAT3 CGameObject::GetLook() 
{ 	
	XMFLOAT3 xmf3Look(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3Look = Vector3::Normalize(xmf3Look);
	return(xmf3Look);
}

XMFLOAT3 CGameObject::GetUp() 
{ 	
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{ 	
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Look, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& rotate)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(rotate.x, rotate.y, rotate.z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fDistance)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fDistance, m_xmf4x4World._42 + vDirection.y * fDistance, m_xmf4x4World._43 + vDirection.z * fDistance);
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);
	
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	if (m_pMesh)
	{
		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
		m_pMesh->Render(hDCFrameBuffer, m_xmf4x4World, pCamera);
		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}

// test
void CGameObject::show() {
	/*float lookx = m_xmf3Look.x;
	float looky = m_xmf3Look.y;
	float lookz = m_xmf3Look.z;

	float upx = m_xmf3Up.x;
	float upy = m_xmf3Up.y;
	float upz = m_xmf3Up.z;

	float rightx = m_xmf3Right.x;
	float righty = m_xmf3Right.y;
	float rightz = m_xmf3Right.z;*/

	std::cout << "look : " << m_xmf3Look.x << " / " << m_xmf3Look.y << " / " << m_xmf3Look.z << std::endl;
	std::cout << "up : " << m_xmf3Up.x << " / " << m_xmf3Up.y << " / " << m_xmf3Up.z << std::endl;
	std::cout << "look : " << m_xmf3Right.x << " / " << m_xmf3Right.y << " / " << m_xmf3Right.z << std::endl;
}