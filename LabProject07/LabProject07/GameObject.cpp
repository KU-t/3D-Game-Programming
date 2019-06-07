#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject() {
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

GameObject::~GameObject() {
	if (m_pMesh) 
		m_pMesh->Release();

}

void GameObject::SetShader(Shader *pShader) {

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

void GameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	OnPrepareRender();
	//게임 객체에 셰이더 객체가 연결되어 있으면 셰이더 상태 객체를 설정한다.
	
	//게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링한다.
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}


