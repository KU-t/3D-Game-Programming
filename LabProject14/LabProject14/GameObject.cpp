#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

GameObject::GameObject(int nMeshes) {
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0) {
		m_ppMeshes = new Mesh*[m_nMeshes];

		for (int i = 0; i < m_nMeshes; i++)
			m_ppMeshes[i] = NULL;
	}

}

GameObject::~GameObject() {
	if (m_ppMeshes)	{
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Release();

			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_pShader) {
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}

}

void GameObject::SetShader(Shader *pShader) {
	if (m_pShader) m_pShader->Release();
	//m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void GameObject::SetMesh(int nIndex, Mesh *pMesh) {
	if (m_ppMeshes) {
		if (m_ppMeshes[nIndex])
			m_ppMeshes[nIndex]->Release();
		
		m_ppMeshes[nIndex] = pMesh;
		
		if (pMesh)
			pMesh->AddRef();
	}

}

void GameObject::ReleaseUploadBuffers() {
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i])
				m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

void GameObject::Animate(float fTimeElapsed) {

}

void GameObject::OnPrepareRender() {

}

void GameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, Camera *pCamera) {
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);
	//게임 객체가 포함하는 모든 메쉬를 렌더링한다.

	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
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

RotatingObject::RotatingObject(int nMeshes) : GameObject(nMeshes) {
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
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

HeightMapTerrain::HeightMapTerrain(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	ID3D12RootSignature * pd3dGraphicsRootSignature, LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength,
	XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color){

	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth; m_nLength = nLength;

	/*지형 객체는 격자 메쉬들의 배열로 만들 것이다.
	nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크 기이다.
	cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new HeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다.
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
	m_nMeshes = cxBlocks * czBlocks;
	
	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다.
	m_ppMeshes = new Mesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)
		m_ppMeshes[i] = NULL;

	HeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++) {
		for (int x = 0, xStart = 0; x < cxBlocks; x++) { 

			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다.
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다.
			pHeightMapGridMesh = new HeightMapGridMesh(pd3dDevice, pd3dCommandList,
				xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}
		//지형을 렌더링하기 위한 셰이더를 생성한다.
	TerrainShader *pShader = new TerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader); 
}

HeightMapTerrain::~HeightMapTerrain(){
	if (m_pHeightMapImage)
		delete m_pHeightMapImage;
}
