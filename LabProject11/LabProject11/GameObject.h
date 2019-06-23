#pragma once
#include "Mesh.h"
#include "Camera.h"

class Shader;

class GameObject{
public:
	GameObject();
	virtual ~GameObject();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() {
		if (--m_nReferences <= 0)
			delete this;
	}

protected: 
	XMFLOAT4X4 m_xmf4x4World;
	Mesh *m_pMesh = NULL;
	Shader *m_pShader = NULL;

public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(Mesh *pMesh);
	virtual void SetShader(Shader *pShader);
	
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	
virtual void Render(ID3D12GraphicsCommandList *pd3dCommandListt, Camera *pCamera);
	
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

public:
	// 상수 버퍼를 생성
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	
	// 상수 버퍼의 내용을 갱신 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	// 게임 객체의 월드 변환 행렬에서 위치 벡터, 방향 벡터 반환 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	// 게임 객체의 위치를 설정
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	// 게임 객체를 로컬 (x-축, y-축, z-축) 방향이동
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	// 게임 객체를 회전 (x-축, y-축, z-축) 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
};

class RotatingObject : public GameObject {
public: 
	RotatingObject(); 
	virtual ~RotatingObject();

private: 
	XMFLOAT3 m_xmf3RotationAxis; float m_fRotationSpeed;

public: 
	void SetRotationSpeed(float fRotationSpeed) {m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
		
	virtual void Animate(float fTimeElapsed);
};