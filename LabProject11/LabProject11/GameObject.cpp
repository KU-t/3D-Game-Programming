#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

GameObject::GameObject() {
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

GameObject::~GameObject() {
	if (m_pMesh) 
		m_pMesh->Release();
}

void GameObject::SetShader(Shader *pShader) {
	if (m_pShader) m_pShader->Release();
	//m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void GameObject::SetMesh(Mesh *pMesh) {
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::ReleaseUploadBuffers() {
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void GameObject::Animate(float fTimeElapsed) {

}

void GameObject::OnPrepareRender() {

}

void GameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera) {
	OnPrepareRender();
	
	// 객체의 정보를 셰이더 변수(상수 버퍼)로 복사
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader) {
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		m_pShader->Render(pd3dCommandList, pCamera);
	}

	//게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링한다.
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList);
}

void GameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle) {
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void GameObject::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList){

}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList){
	XMFLOAT4X4 xmf4x4World;
	// 현재 위치 저장
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	
	// 객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 
}

void GameObject::ReleaseShaderVariables(){

}

XMFLOAT3 GameObject::GetPosition(){
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

// 게임 객체의 로컬 z-축 벡터를 반환
XMFLOAT3 GameObject::GetLook(){
	XMFLOAT3 Look(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(Look));
}

// 게임 객체의 로컬 y-축 벡터를 반환
XMFLOAT3 GameObject::GetUp(){
	XMFLOAT3 UP(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	return(Vector3::Normalize(UP));
}

// 게임 객체의 로컬 x-축 벡터를 반환
XMFLOAT3 GameObject::GetRight(){
	XMFLOAT3 RIGHT(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return(Vector3::Normalize(RIGHT));
}

RotatingObject::RotatingObject() {
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

// 게임 객체를 로컬 x-축 방향으로 이동
void GameObject::MoveStrafe(float fDistance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	GameObject::SetPosition(xmf3Position);
}

// 게임 객체를 로컬 y-축 방향으로 이동
void GameObject::MoveUp(float fDistance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	GameObject::SetPosition(xmf3Position);
}

// 게임 객체를 로컬 z-축 방향으로 이동
void GameObject::MoveForward(float fDistance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	GameObject::SetPosition(xmf3Position);
}

// 게임 객체를 주어진 각도로 회전
void GameObject::Rotate(float fPitch, float fYaw, float fRoll) {
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

RotatingObject::~RotatingObject() {

}

void RotatingObject::Animate(float fTimeElapsed) {
	GameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

void GameObject::SetPosition(float x, float y, float z){
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;

}

void GameObject::SetPosition(XMFLOAT3 xmf3Position){
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
