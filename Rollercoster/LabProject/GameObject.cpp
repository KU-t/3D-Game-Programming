#include "StdAfx.h"
#include "GameObject.h"

CGameObject::CGameObject() { 
	//메쉬
	m_pMesh = NULL; 

	//월드 행렬
	m_xmf4x4World = Matrix4x4::Identity();

	//Look Up Right
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);

	// color
	m_dwColor = RGB(0, 0, 0);
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject(){
	m_pMesh = pMesh; 
}

CGameObject::~CGameObject(void){
	if (m_pMesh) 
		m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z) {
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) { 
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

XMFLOAT3 CGameObject::GetPosition() { 
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}

XMFLOAT3 CGameObject::GetLook() { 	
	XMFLOAT3 xmf3Look(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3Look = Vector3::Normalize(xmf3Look);
	return(xmf3Look);
}

XMFLOAT3 CGameObject::GetUp() { 	
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight(){ 	
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll){
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& rotate){
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(rotate.x, rotate.y, rotate.z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle){
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fDistance){
	// 기존 위치 + (방향 * 이동거리)
	SetPosition(m_xmf4x4World._41 + vDirection.x * fDistance, m_xmf4x4World._42 + vDirection.y * fDistance, m_xmf4x4World._43 + vDirection.z * fDistance);
}

void CGameObject::Animate(float fElapsedTime){
	
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera){
	if (m_pMesh)	{
		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
		m_pMesh->Render(hDCFrameBuffer, m_xmf4x4World, pCamera);
		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}
