#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

Player::Player(int nMeshes) : GameObject(nMeshes) {

	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f; m_fRoll = 0.0f; m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

}

Player::~Player(){
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

/*플레이어의 위치를 변경하는 3가지 함수이다.
플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를 누를 때 변경된다.
플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/
void Player::Move(ULONG nDirection, float fDistance, bool bVelocity){
	if (nDirection)	{

		// 이동할 위치
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다.
		if (nDirection & DIR_FORWARD)	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (nDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (nDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (nDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다. ‘Page Down’을 누르면 반대 방향으로 이동한다.
		if (nDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (nDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
		Move(xmf3Shift, bVelocity);
	}
}

void Player::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) {
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다.
	if (bUpdateVelocity) {
		//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다.
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	} 
	
	else {
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다.
		XMFLOAT3 Shiftfloat3(xmf3Shift);
		m_pCamera->Move(Shiftfloat3);
	}
}

void Player::Move(float fxOffset, float fyOffset, float fzOffset){

}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.
void Player::Rotate(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f) {

	DWORD nCameraMode = m_pCamera->GetMode();

	//1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다.

	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA)) {
		/*로컬 x-축을 중심으로 회전하는 것은 고개를 앞뒤로 숙이는 동작에 해당한다.
		그러므로 x-축을 중심으로 회전하는 각도는 -89.0~+89.0도 사이로 제한한다.
		x는 현재의 m_fPitch에서 실제 회전하는 각도이므로
		x만큼 회전한 다음 Pitch가 +89도 보다 크거나 -89도 보다 작으면
		m_fPitch가 +89도 또는 -89도가 되도록 회전각도(x)를 수정한다.*/
		if (fxOffset != 0.0f) {
			m_fPitch += fxOffset;

			if (m_fPitch > +89.0f) {
				fxOffset -= (m_fPitch - 89.0f);
				m_fPitch = +89.0f;
			}

			if (m_fPitch < -89.0f) {
				fxOffset -= (m_fPitch + 89.0f);
				m_fPitch = -89.0f;
			}
		}

		if (fyOffset != 0.0f) {
			//로컬 y-축을 중심으로 회전하는 것은 몸통을 돌리는 것이므로 회전 각도의 제한이 없다.
			m_fYaw += fyOffset;
			if (m_fYaw > 360.0f)	m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f)		m_fYaw += 360.0f;
		}

		if (fzOffset != 0.0f) {
			/*로컬 z-축을 중심으로 회전하는 것은 몸통을 좌우로 기울이는 것이므로 회전 각도는 -20.0~+20.0도 사이로 제한된다.
			z는 현재의 m_fRoll에서 실제 회전하는 각도이므로
			z만큼 회전한 다음 m_fRoll이 +20도 보다 크거나 -20도보다 작으면 m_fRoll이 +20도 또는 -20도가 되도록 회전각도(z)를 수정한다.*/
			m_fRoll += fzOffset;
			if (m_fRoll > +20.0f) {
				fzOffset -= (m_fRoll - 20.0f);
				m_fRoll = +20.0f;
			}
			if (m_fRoll < -20.0f) {
				fzOffset -= (m_fRoll + 20.0f);
				m_fRoll = -20.0f;
			}
		}

		//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다.
		m_pCamera->Rotate(fxOffset, fyOffset, fzOffset);

		/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다.
		플레이어 의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다.
		기본적으로 Up 벡 터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
		if (fyOffset != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fyOffset));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	else if (nCameraMode == SPACESHIP_CAMERA) {
		/*스페이스-쉽 카메라에서 플레이어의 회전은 회전 각도의 제한이 없다.
		그리고 모든 축을 중심으로 회전을 할 수 있 다.*/ 
		m_pCamera->Rotate(fxOffset, fyOffset, fzOffset);
		
		if (fxOffset != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fxOffset));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		
		if (fyOffset != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fyOffset));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		} 
		
		if (fzOffset != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fzOffset));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	
	/*회전으로 인해 플레이어의 로컬 x-축, y-축, z-축이 서로 직교하지 않을 수 있으므로
	z-축(LookAt 벡터)을 기준으 로 하여 서로 직교하고 단위벡터가 되도록 한다.*/ 
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true); 
}

//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다. 
void Player::Update(float fTimeElapsed){
	/*플레이어의 속도 벡터를 중력 벡터와 더한다.
	중력 벡터에 fTimeElapsed를 곱하는 것은 중력을 시간에 비례하도록 적용한다는 의미이다.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	/*플레이어의 속도 벡터의 XZ-성분의 크기를 구한다.
	이것이 XZ-평면의 최대 속력보다 크면 속도 벡터의 x와 z-방향 성분을 조정한다.*/
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ) {
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	
	/*플레이어의 속도 벡터의 y-성분의 크기를 구한다.
	이것이 y-축 방향의 최대 속력보다 크면 속도 벡터의 y-방향 성	분을 조정한다.*/
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다).
	Move(m_xmf3Velocity, false);

	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다.
	플레이어의 새로운 위치가 유효한 위치가 아닐 수도 있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다.
	이러한 상황에서 플레이어의 위치를 유효한 위치로 다시 변경할 수 있다.*/ 
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCameraMode = m_pCamera->GetMode();

	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다.
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다.
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	
	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	
	//카메라의 카메라 변환 행렬을 다시 생성한다.
	m_pCamera->RegenerateViewMatrix();

	/*플레이어의 속도 벡터가 마찰력 때문에 감속이 되어야 한다면 감속 벡터를 생성한다.
	속도 벡터의 반대 방향 벡터를 구하고 단위 벡터로 만든다.
	마찰 계수를 시간에 비례하도록 하여 마찰력을 구한다.
	단위 벡터에 마찰력을 곱하여 감 속 벡터를 구한다. 
	속도 벡터에 감속 벡터를 더하여 속도 벡터를 줄인다.
	마찰력이 속력보다 크면 속력은 0이 될 것이다.*/
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void Player::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList){
	GameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Player::ReleaseShaderVariables(){
	GameObject::ReleaseShaderVariables();
	
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void Player::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList){
	GameObject::UpdateShaderVariables(pd3dCommandList);
}

/*카메라를 변경할 때 ChangeCamera() 함수에서 호출되는 함수이다.
nCurrentCameraMode는 현재 카메라의 모드 이고 nNewCameraMode는 새로 설정할 카메라 모드이다.*/
Camera * Player::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode){
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다.
	Camera *pNewCamera = NULL;
	
	switch (nNewCameraMode) {
		case FIRST_PERSON_CAMERA:
			pNewCamera = new FirstPersonCamera(m_pCamera);
			break;
		
		case SPACESHIP_CAMERA:
			pNewCamera = new SpaceShipCamera(m_pCamera); 
				break;

		case THIRD_PERSON_CAMERA:
			pNewCamera = new ThirdPersonCamera(m_pCamera);
			break;		
	} 
	
	/*현재 카메라의 모드가 스페이스-쉽 모드의 카메라이고 새로운 카메라가 1인칭 또는 3인칭 카메라이면
	플레이어의 Up 벡터를 월드좌표계의 y-축 방향 벡터(0, 1, 0)이 되도록 한다.
	즉, 똑바로 서도록 한다. 그리고 스페이스-쉽 카메 라의 경우 플레이어의 이동에는 제약이 없다.
	특히, y-축 방향의 움직임이 자유롭다.
	그러므로 플레이어의 위치는 공 중(위치 벡터의 y-좌표가 0보다 크다)이 될 수 있다.
	이때 새로운 카메라가 1인칭 또는 3인칭 카메라이면 플레이어의 위치는 지면이 되어야 한다.
	그러므로 플레이어의 Right 벡터와 Look 벡터의 y 값을 0으로 만든다.
	이제 플레이어의 Right 벡터와 Look 벡터는 단위벡터가 아니므로 정규화한다.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA) {
		
		XMFLOAT3 RIGHTNorm{ m_xmf3Right.x, 0.0f, m_xmf3Right.z };
		m_xmf3Right = Vector3::Normalize(RIGHTNorm);
		
		XMFLOAT3 UPNorm{ 0.0f, 1.0f, 0.0f };
		m_xmf3Up = Vector3::Normalize(UPNorm);
		
		XMFLOAT3 LOOKNorm{ m_xmf3Look.x, 0.0f, m_xmf3Look.z };
		m_xmf3Look = Vector3::Normalize(LOOKNorm);
		
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		
		/*Look 벡터와 월드좌표계의 z-축(0, 0, 1)이 이루는 각도(내적=cos)를 계산하여 플레이어의 y-축의 회전 각도 m_fYaw로 설정한다.*/ 
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}

	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera) {
		/*새로운 카메라의 모드가 스페이스-쉽 모드의 카메라이고 현재 카메라 모드가 1인칭 또는 3인칭 카메라이면
		플레이 어의 로컬 축을 현재 카메라의 로컬 축과 같게 만든다.*/
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera) {
		pNewCamera->SetMode(nNewCameraMode);
		
		//현재 카메라를 사용하는 플레이어 객체를 설정한다.
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) 
		delete m_pCamera;

	return(pNewCamera);
}

void Player::OnPrepareRender(){
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;

	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;

	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;

	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void Player::Render(ID3D12GraphicsCommandList * pd3dCommandList, Camera * pCamera){
	
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	
	if (m_pShader) {
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		//m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		//m_pShader->Render(pd3dCommandList, pCamera);
	}

	// 카메라 모드가 3인칭이면 플레이어 객체를 렌더링한다.
	if (nCameraMode == THIRD_PERSON_CAMERA) 
		GameObject::Render(pd3dCommandList, pCamera); 
}

AirplanePlayer::AirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes){
	//비행기 메쉬를 생성한다.
	Mesh *pAirplaneMesh = new AirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f));
	SetMesh(0, pAirplaneMesh);
	
	//플레이어의 카메라를 스페이스-쉽 카메라로 변경(생성)한다.
	m_pCamera = ChangeCamera(SPACESHIP_CAMERA/*THIRD_PERSON_CAMERA*/, 0.0f);

	//플레이어를 위한 셰이더 변수를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//플레이어의 위치를 설정한다. 
	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));

	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	PlayerShader *pShader = new PlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	// 이거 뭐지(?)  - 쉐이더에 객체가 있는데
	SetShader(pShader); 
}

AirplanePlayer::~AirplanePlayer(){

}

//카메라를 변경할 때 호출되는 함수이다. nNewCameraMode는 새로 설정할 카메라 모드이다. 
Camera * AirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed){
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) 
		return(m_pCamera);
	
	switch (nNewCameraMode) {
		case FIRST_PERSON_CAMERA:
			//플레이어의 특성을 1인칭 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
			SetFriction(200.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(125.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);

			// 1인칭 카메라의 지연 효과를 설정한다. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.0f);
			// 카메라가 캐릭터기준 어디에 위치하는가
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
			// 카메라 절두체
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// 카메라 뷰포트
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// 카메라 씨저
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
			break;
		
		case SPACESHIP_CAMERA:
			//플레이어의 특성을 스페이스-쉽 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
			SetFriction(125.0f); 
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(400.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);

			// 자유시점 카메라의 지연 효과를 설정한다. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.0f);
			// 카메라가 캐릭터기준 어디에 위치하는가
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			// 카메라 절두체
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// 카메라 뷰포트
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// 카메라 씨저
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
			break; 
		
		case THIRD_PERSON_CAMERA: 
			//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
			SetFriction(250.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(125.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			
			// 3인칭 카메라의 지연 효과를 설정한다. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.25f);
			// 카메라가 캐릭터기준 어디에 위치하는가
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -50.0f));
			// 카메라 절두체
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// 카메라 뷰포트
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// 카메라 씨저
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		
		default:
			break; 
	}
	
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);

	return(m_pCamera);
}

void AirplanePlayer::OnPrepareRender(){
	Player::OnPrepareRender();

	//비행기 모델을 그리기 전, 회전
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

TerrainPlayer::TerrainPlayer(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	ID3D12RootSignature * pd3dGraphicsRootSignature, void * pContext, int nMeshes){

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)pContext;

	/*플레이어의 위치를 지형의 가운데(y-축 좌표는 지형의 높이보다 1500 높게)로 설정한다.
	플레이어 위치 벡터의 y좌표가 지형의 높이보다 크고 중력이 작용하도록 플레이어를 설정하였으므로
	플레이어는 점차적으로 하강하게 된다.*/
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth()*0.5f, pTerrain->GetLength()*0.5f);
	SetPosition(XMFLOAT3(pTerrain->GetWidth()*0.5f, fHeight + 1500.0f, pTerrain->GetLength()*0.5f));
	
	//플레이어의 위치가 변경될 때 지형의 정보에 따라 플레이어의 위치를 변경할 수 있도록 설정한다.
	SetPlayerUpdatedContext(pTerrain);
	
	//카메라의 위치가 변경될 때 지형의 정보에 따라 카메라의 위치를 변경할 수 있도록 설정한다.
	SetCameraUpdatedContext(pTerrain);

	CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 12.0f, 4.0f);
	SetMesh(0, pCubeMesh);

	//플레이어를 렌더링할 셰이더를 생성한다.
	PlayerShader *pShader = new PlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

TerrainPlayer::~TerrainPlayer(){

}

Camera * TerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed){
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode)
		return(m_pCamera);
	
	switch (nNewCameraMode) {
		case FIRST_PERSON_CAMERA:
			SetFriction(250.0f);

			//1인칭 카메라일 때 플레이어에 y-축 방향으로 중력이 작용한다.
			SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
		
		case SPACESHIP_CAMERA:
			SetFriction(125.0f);
			//스페이스 쉽 카메라일 때 플레이어에 중력이 작용하지 않는다.
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
		
		case THIRD_PERSON_CAMERA:
			SetFriction(250.0f);
			//3인칭 카메라일 때 플레이어에 y-축 방향으로 중력이 작용한다.
			SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f); 
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.25f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;

		default:
			break;
	}
	Update(fTimeElapsed);
	
	return(m_pCamera);
}

void TerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed){
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)m_pPlayerUpdatedContext;
	/*지형에서 플레이어의 현재 위치 (x, z)의 지형 높이(y)를 구한다.
	그리고 플레이어 메쉬의 높이가 12이고 플레이어의 중심이 직육면체의 가운데이므로
	y 값에 메쉬의 높이의 절반을 더하면 플레이어의 위치가 된다.*/
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
	
	/*플레이어의 위치 벡터의 y-값이 음수이면(예를 들어, 중력이 적용되는 경우)
	플레이어의 위치 벡터의 y-값이 점점 작아지게 된다.
	이때 플레이어의 현재 위치 벡터의 y 값이 지형의 높이(실제로 지형의 높이 + 6)보다 작으면
	플레이어 의 일부가 지형 아래에 있게 된다.
	이러한 경우를 방지하려면 플레이어의 속도 벡터의 y 값을 0으로 만들고
	플레이어 의 위치 벡터의 y-값을 지형의 높이(실제로 지형의 높이 + 6)로 설정한다.
	그러면 플레이어는 항상 지형 위에 있게 된 다.*/
	if (xmf3PlayerPosition.y < fHeight) {
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
	
		// ???????????????????????
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);

		// ?????????????????????
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void TerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed){
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	/*높이 맵에서 카메라의 현재 위치 (x, z)에 대한 지형의 높이(y 값)를 구한다.
	이 값이 카메라의 위치 벡터의 y-값 보 다 크면 카메라가 지형의 아래에 있게 된다.
	이렇게 되면 다음 그림의 왼쪽과 같이 지형이 그려지지 않는 경우가 발생 한다
	(카메라가 지형 안에 있으므로 삼각형의 와인딩 순서가 바뀐다).
	이러한 경우가 발생하지 않도록 카메라의 위치 벡 터의 y-값의 최소값은	(지형의 높이 + 5)로 설정한다.
	카메라의 위치 벡터의 y-값의 최소값은 지형의 모든 위치에서 카메라가 지형 아래에 위치하지 않도록 설정해야 한다.*/
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)m_pCameraUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight) {
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA) {
			//3인칭 카메라의 경우 카메라 위치(y-좌표)가 변경되었으므로 카메라가 플레이어를 바라보도록 한다.
			ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
			
			//????????????????????????????????????
			XMFLOAT3 xmGetPosition = GetPosition();
			p3rdPersonCamera->SetLookAt(xmGetPosition);
		}
	} 
}
