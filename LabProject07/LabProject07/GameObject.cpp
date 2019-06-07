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
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void GameObject::Animate(float fTimeElapsed) {

}

void GameObject::OnPrepareRender() {

}

void GameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList) {
	OnPrepareRender();
	//���� ��ü�� ���̴� ��ü�� ����Ǿ� ������ ���̴� ���� ��ü�� �����Ѵ�.
	
	//���� ��ü�� �޽��� ����Ǿ� ������ �޽��� �������Ѵ�.
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}


