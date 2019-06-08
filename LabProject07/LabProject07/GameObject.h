#pragma once
#include "Mesh.h"
#include "Shader.h"

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