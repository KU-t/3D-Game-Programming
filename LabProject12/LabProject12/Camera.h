#pragma once
//프레임 버퍼의 크기와 종횡비(Aspect Ratio)를 나타내는 상수를 다음과 같이 선언한다.
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

//카메라의 종류(모드: Mode)를 나타내는 상수를 다음과 같이 선언한다.
#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA 0x02
#define THIRD_PERSON_CAMERA 0x03

class Player;


struct VS_CB_CAMERA_INFO {
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
};

class Camera {
protected:
	// 카메라의 위치(월드좌표계) 벡터
	XMFLOAT3 m_xmf3Position;

	// 카메라의 로컬 x-축(Right), y-축(Up), z-축(Look)을 나타내는 벡터
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	// 카메라가 x-축, z-축, y-축으로 얼마만큼 회전했는 가를 나타내는 각도
	float m_fPitch;
	float m_fRoll;
	float m_fYaw;
	
	// 카메라의 종류(1인칭 카메라, 스페이스-쉽 카메라, 3인칭 카메라)를 나타낸다.
	DWORD m_nMode;
	
	// 플레이어가 바라볼 위치 벡터이다. 주로 3인칭 카메라에서 사용된다.
	XMFLOAT3 m_xmf3LookAtWorld;
	
	// 플레이어와 카메라의 오프셋을 나타내는 벡터이다. 주로 3인칭 카메라에서 사용된다.
	XMFLOAT3 m_xmf3Offset;
	
	// 플레이어가 회전할 때 얼마만큼의 시간을 지연시킨 후 카메라를 회전시킬 것인가를 나타낸다.
	float m_fTimeLag;
	
	// 카메라 변환 행렬 
	XMFLOAT4X4 m_xmf4x4View;
	// 투영 변환 행렬 
	XMFLOAT4X4 m_xmf4x4Projection;
	// 뷰포트
	D3D12_VIEWPORT m_d3dViewport;
	// 씨저 사각형
	D3D12_RECT m_d3dScissorRect;

	// 카메라를 가지고 있는 플레이어에 대한 포인터
	Player *m_pPlayer = NULL;

public: 
	Camera();
	Camera(Camera *pCamera);
	virtual ~Camera();

	// 카메라의 정보를 셰이더 프로그램에게 전달하기 위한 상수 버퍼
	// 상수 버퍼 - 생성
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	// 상수 버퍼 - 제거
	virtual void ReleaseShaderVariables();
	// 상수 버퍼 - 갱신
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	
	// 카메라 변환 행렬 생성
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	
	/*카메라가 여러번 회전을 하게 되면 누적된 실수 연산의 부정확성 때문에
	카메라의 로컬 x-축(Right), y-축(Up), z축(LookAt)이 서로 직교하지 않을 수 있다.
	카메라의 로컬 x-축(Right), y-축(Up), z-축(LookAt)이 서로 직교하도록 만들어준다.*/
	// 카메라 로컬 x,y,z 축을 직교하게 변경
	void RegenerateViewMatrix();

	// 카메라 뷰포트 생성
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	// 카메라 씨저 생성
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);

	// Player Set & Get
	void SetPlayer(Player *pPlayer) { m_pPlayer = pPlayer; }
	Player *GetPlayer() { return(m_pPlayer); }

	// Mode Set & Get
	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	// 위치 Set & Get
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }

	// Look위치 Set & Get
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	// 카메라 x,y,z 축 Get
	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	// 카메라 x,y,z 누적 회전량 Get
	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	// Player와 카메라의 offset Get
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	// 카메라 회전의 지연시간 Set & Get
	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	// 카메라 변환 행렬 Get
	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	// 카메라 투영 행렬 Get
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	// 카메라 뷰 포트 Get
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	// 카메라 씨저 사각형 Get
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	// 카메라를 xmf3Shift 만큼 추가로 이동
	virtual void Move(XMFLOAT3& xmf3Shift) {
		m_xmf3Position.x += xmf3Shift.x;
		m_xmf3Position.y += xmf3Shift.y;
		m_xmf3Position.z += xmf3Shift.z;
	}
	
	// 카메라를 x-축, y-축, z-축으로 회전하는 가상함수
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }

	// 카메라의 이동, 회전에 따라 카메라의 정보를 갱신하는 가상함수
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { } 
	
	// 3인칭 카메라에서 카메라가 바라보는 지점을 설정. 일반적으로 플레이어를 바라보도록 설정
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) { } 
};

class SpaceShipCamera : public Camera {
public:
	SpaceShipCamera(Camera *pCamera);
	virtual ~SpaceShipCamera() { }
	
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class FirstPersonCamera : public Camera {
public:
	FirstPersonCamera(Camera *pCamera);
	virtual ~FirstPersonCamera() { }
	
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class ThirdPersonCamera : public Camera {
public:
	ThirdPersonCamera(Camera *pCamera);
	virtual ~ThirdPersonCamera() { }
	
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};