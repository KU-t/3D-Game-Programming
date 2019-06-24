#pragma once
#include "Mesh.h"
#include "Camera.h"

class Shader;

class GameObject{
public:
	GameObject(int nMeshes = 1);

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
	
	Shader *m_pShader = NULL;

	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다.
	Mesh **m_ppMeshes = NULL;
	int m_nMeshes = 0;


public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(int nIndex, Mesh *pMesh);
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
	RotatingObject(int nMeshes = 1);
	virtual ~RotatingObject();

private: 
	XMFLOAT3 m_xmf3RotationAxis; float m_fRotationSpeed;

public: 
	void SetRotationSpeed(float fRotationSpeed) {m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
		
	virtual void Animate(float fTimeElapsed);
};

class HeightMapTerrain : public GameObject {
public:
	HeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName,
		int nWidth, int nLength, int nBlockWidth, int nBlockLength,
		XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~HeightMapTerrain();

private:
	//지형의 높이 맵으로 사용할 이미지이다.
	HeightMapImage *m_pHeightMapImage;

	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth; int m_nLength;

	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;

public:
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	} 
	
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() {
		return(m_pHeightMapImage->GetHeightMapWidth());
	}
	
	int GetHeightMapLength() {
		return(m_pHeightMapImage->GetHeightMapLength());
	}

	XMFLOAT3 GetScale() { return(m_xmf3Scale); } 
	
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
