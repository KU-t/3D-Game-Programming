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

/*�÷��̾��� ��ġ�� �����ϴ� 3���� �Լ��̴�.
�÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带 ���� �� ����ȴ�.
�÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/
void Player::Move(ULONG nDirection, float fDistance, bool bVelocity){
	if (nDirection)	{

		// �̵��� ��ġ
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		
		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (nDirection & DIR_FORWARD)	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (nDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		//ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (nDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (nDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//��Page Up���� ������ ���� y-�� �������� �̵��Ѵ�. ��Page Down���� ������ �ݴ� �������� �̵��Ѵ�.
		if (nDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (nDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		Move(xmf3Shift, bVelocity);
	}
}

void Player::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) {
	//bUpdateVelocity�� ���̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� �����Ѵ�.
	if (bUpdateVelocity) {
		//�÷��̾��� �ӵ� ���͸� xmf3Shift ���͸�ŭ �����Ѵ�.
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	} 
	
	else {
		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		XMFLOAT3 Shiftfloat3(xmf3Shift);
		m_pCamera->Move(Shiftfloat3);
	}
}

void Player::Move(float fxOffset, float fyOffset, float fzOffset){

}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
void Player::Rotate(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f) {

	DWORD nCameraMode = m_pCamera->GetMode();

	//1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������.

	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA)) {
		/*���� x-���� �߽����� ȸ���ϴ� ���� ���� �յڷ� ���̴� ���ۿ� �ش��Ѵ�.
		�׷��Ƿ� x-���� �߽����� ȸ���ϴ� ������ -89.0~+89.0�� ���̷� �����Ѵ�.
		x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ�
		x��ŭ ȸ���� ���� Pitch�� +89�� ���� ũ�ų� -89�� ���� ������
		m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.*/
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
			//���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����.
			m_fYaw += fyOffset;
			if (m_fYaw > 360.0f)	m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f)		m_fYaw += 360.0f;
		}

		if (fzOffset != 0.0f) {
			/*���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�.
			z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ�
			z��ŭ ȸ���� ���� m_fRoll�� +20�� ���� ũ�ų� -20������ ������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.*/
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

		//ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�.
		m_pCamera->Rotate(fxOffset, fyOffset, fzOffset);

		/*�÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��.
		�÷��̾� �� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��.
		�⺻������ Up �� �͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.*/
		if (fyOffset != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fyOffset));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	else if (nCameraMode == SPACESHIP_CAMERA) {
		/*�����̽�-�� ī�޶󿡼� �÷��̾��� ȸ���� ȸ�� ������ ������ ����.
		�׸��� ��� ���� �߽����� ȸ���� �� �� �� ��.*/ 
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
	
	/*ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ�
	z-��(LookAt ����)�� ������ �� �Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.*/ 
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true); 
}

//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�. 
void Player::Update(float fTimeElapsed){
	/*�÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ���Ѵ�.
	�߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ��̴�.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	/*�÷��̾��� �ӵ� ������ XZ-������ ũ�⸦ ���Ѵ�.
	�̰��� XZ-����� �ִ� �ӷº��� ũ�� �ӵ� ������ x�� z-���� ������ �����Ѵ�.*/
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ) {
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	
	/*�÷��̾��� �ӵ� ������ y-������ ũ�⸦ ���Ѵ�.
	�̰��� y-�� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ��	���� �����Ѵ�.*/
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�).
	Move(m_xmf3Velocity, false);

	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�.
	�÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ���� �ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�.
	�̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ� ������ �� �ִ�.*/ 
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCameraMode = m_pCamera->GetMode();

	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�.
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�.
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	
	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	
	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�.
	m_pCamera->RegenerateViewMatrix();

	/*�÷��̾��� �ӵ� ���Ͱ� ������ ������ ������ �Ǿ�� �Ѵٸ� ���� ���͸� �����Ѵ�.
	�ӵ� ������ �ݴ� ���� ���͸� ���ϰ� ���� ���ͷ� �����.
	���� ����� �ð��� ����ϵ��� �Ͽ� �������� ���Ѵ�.
	���� ���Ϳ� �������� ���Ͽ� �� �� ���͸� ���Ѵ�. 
	�ӵ� ���Ϳ� ���� ���͸� ���Ͽ� �ӵ� ���͸� ���δ�.
	�������� �ӷº��� ũ�� �ӷ��� 0�� �� ���̴�.*/
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

/*ī�޶� ������ �� ChangeCamera() �Լ����� ȣ��Ǵ� �Լ��̴�.
nCurrentCameraMode�� ���� ī�޶��� ��� �̰� nNewCameraMode�� ���� ������ ī�޶� ����̴�.*/
Camera * Player::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode){
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�.
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
	
	/*���� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸�
	�÷��̾��� Up ���͸� ������ǥ���� y-�� ���� ����(0, 1, 0)�� �ǵ��� �Ѵ�.
	��, �ȹٷ� ������ �Ѵ�. �׸��� �����̽�-�� ī�� ���� ��� �÷��̾��� �̵����� ������ ����.
	Ư��, y-�� ������ �������� �����Ӵ�.
	�׷��Ƿ� �÷��̾��� ��ġ�� �� ��(��ġ ������ y-��ǥ�� 0���� ũ��)�� �� �� �ִ�.
	�̶� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ��ġ�� ������ �Ǿ�� �Ѵ�.
	�׷��Ƿ� �÷��̾��� Right ���Ϳ� Look ������ y ���� 0���� �����.
	���� �÷��̾��� Right ���Ϳ� Look ���ʹ� �������Ͱ� �ƴϹǷ� ����ȭ�Ѵ�.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA) {
		
		XMFLOAT3 RIGHTNorm{ m_xmf3Right.x, 0.0f, m_xmf3Right.z };
		m_xmf3Right = Vector3::Normalize(RIGHTNorm);
		
		XMFLOAT3 UPNorm{ 0.0f, 1.0f, 0.0f };
		m_xmf3Up = Vector3::Normalize(UPNorm);
		
		XMFLOAT3 LOOKNorm{ m_xmf3Look.x, 0.0f, m_xmf3Look.z };
		m_xmf3Look = Vector3::Normalize(LOOKNorm);
		
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		
		/*Look ���Ϳ� ������ǥ���� z-��(0, 0, 1)�� �̷�� ����(����=cos)�� ����Ͽ� �÷��̾��� y-���� ȸ�� ���� m_fYaw�� �����Ѵ�.*/ 
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}

	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera) {
		/*���ο� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���� ī�޶� ��尡 1��Ī �Ǵ� 3��Ī ī�޶��̸�
		�÷��� ���� ���� ���� ���� ī�޶��� ���� ��� ���� �����.*/
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera) {
		pNewCamera->SetMode(nNewCameraMode);
		
		//���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�.
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
		//���� ��ü�� ���� ��ȯ ����� ���̴��� ��� ���۷� ����(����)�Ѵ�.
		//m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		//m_pShader->Render(pd3dCommandList, pCamera);
	}

	// ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�.
	if (nCameraMode == THIRD_PERSON_CAMERA) 
		GameObject::Render(pd3dCommandList, pCamera); 
}

AirplanePlayer::AirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes){
	//����� �޽��� �����Ѵ�.
	Mesh *pAirplaneMesh = new AirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f));
	SetMesh(0, pAirplaneMesh);
	
	//�÷��̾��� ī�޶� �����̽�-�� ī�޶�� ����(����)�Ѵ�.
	m_pCamera = ChangeCamera(SPACESHIP_CAMERA/*THIRD_PERSON_CAMERA*/, 0.0f);

	//�÷��̾ ���� ���̴� ������ �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//�÷��̾��� ��ġ�� �����Ѵ�. 
	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));

	//�÷��̾�(�����) �޽��� �������� �� ����� ���̴��� �����Ѵ�.
	PlayerShader *pShader = new PlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

	// �̰� ����(?)  - ���̴��� ��ü�� �ִµ�
	SetShader(pShader); 
}

AirplanePlayer::~AirplanePlayer(){

}

//ī�޶� ������ �� ȣ��Ǵ� �Լ��̴�. nNewCameraMode�� ���� ������ ī�޶� ����̴�. 
Camera * AirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed){
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) 
		return(m_pCamera);
	
	switch (nNewCameraMode) {
		case FIRST_PERSON_CAMERA:
			//�÷��̾��� Ư���� 1��Ī ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
			SetFriction(200.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(125.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);

			// 1��Ī ī�޶��� ���� ȿ���� �����Ѵ�. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.0f);
			// ī�޶� ĳ���ͱ��� ��� ��ġ�ϴ°�
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
			// ī�޶� ����ü
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// ī�޶� ����Ʈ
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// ī�޶� ����
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
			break;
		
		case SPACESHIP_CAMERA:
			//�÷��̾��� Ư���� �����̽�-�� ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
			SetFriction(125.0f); 
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(400.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);

			// �������� ī�޶��� ���� ȿ���� �����Ѵ�. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.0f);
			// ī�޶� ĳ���ͱ��� ��� ��ġ�ϴ°�
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			// ī�޶� ����ü
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// ī�޶� ����Ʈ
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// ī�޶� ����
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
			break; 
		
		case THIRD_PERSON_CAMERA: 
			//�÷��̾��� Ư���� 3��Ī ī�޶� ��忡 �°� �����Ѵ�. ���� ȿ���� ī�޶� �������� �����Ѵ�.
			SetFriction(250.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(125.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			
			// 3��Ī ī�޶��� ���� ȿ���� �����Ѵ�. (0.f ~ 1.f)
			m_pCamera->SetTimeLag(0.25f);
			// ī�޶� ĳ���ͱ��� ��� ��ġ�ϴ°�
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -50.0f));
			// ī�޶� ����ü
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			// ī�޶� ����Ʈ
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			// ī�޶� ����
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		
		default:
			break; 
	}
	
	//�÷��̾ �ð��� ����� ���� ����(��ġ�� ������ ����: �ӵ�, ������, �߷� ���� ó��)�Ѵ�.
	Update(fTimeElapsed);

	return(m_pCamera);
}

void AirplanePlayer::OnPrepareRender(){
	Player::OnPrepareRender();

	//����� ���� �׸��� ��, ȸ��
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

TerrainPlayer::TerrainPlayer(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	ID3D12RootSignature * pd3dGraphicsRootSignature, void * pContext, int nMeshes){

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)pContext;

	/*�÷��̾��� ��ġ�� ������ ���(y-�� ��ǥ�� ������ ���̺��� 1500 ����)�� �����Ѵ�.
	�÷��̾� ��ġ ������ y��ǥ�� ������ ���̺��� ũ�� �߷��� �ۿ��ϵ��� �÷��̾ �����Ͽ����Ƿ�
	�÷��̾�� ���������� �ϰ��ϰ� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth()*0.5f, pTerrain->GetLength()*0.5f);
	SetPosition(XMFLOAT3(pTerrain->GetWidth()*0.5f, fHeight + 1500.0f, pTerrain->GetLength()*0.5f));
	
	//�÷��̾��� ��ġ�� ����� �� ������ ������ ���� �÷��̾��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	SetPlayerUpdatedContext(pTerrain);
	
	//ī�޶��� ��ġ�� ����� �� ������ ������ ���� ī�޶��� ��ġ�� ������ �� �ֵ��� �����Ѵ�.
	SetCameraUpdatedContext(pTerrain);

	CubeMeshDiffused *pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 12.0f, 4.0f);
	SetMesh(0, pCubeMesh);

	//�÷��̾ �������� ���̴��� �����Ѵ�.
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

			//1��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
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
			//�����̽� �� ī�޶��� �� �÷��̾ �߷��� �ۿ����� �ʴ´�.
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
			//3��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
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
	/*�������� �÷��̾��� ���� ��ġ (x, z)�� ���� ����(y)�� ���Ѵ�.
	�׸��� �÷��̾� �޽��� ���̰� 12�̰� �÷��̾��� �߽��� ������ü�� ����̹Ƿ�
	y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
	
	/*�÷��̾��� ��ġ ������ y-���� �����̸�(���� ���, �߷��� ����Ǵ� ���)
	�÷��̾��� ��ġ ������ y-���� ���� �۾����� �ȴ�.
	�̶� �÷��̾��� ���� ��ġ ������ y ���� ������ ����(������ ������ ���� + 6)���� ������
	�÷��̾� �� �Ϻΰ� ���� �Ʒ��� �ְ� �ȴ�.
	�̷��� ��츦 �����Ϸ��� �÷��̾��� �ӵ� ������ y ���� 0���� �����
	�÷��̾� �� ��ġ ������ y-���� ������ ����(������ ������ ���� + 6)�� �����Ѵ�.
	�׷��� �÷��̾�� �׻� ���� ���� �ְ� �� ��.*/
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
	/*���� �ʿ��� ī�޶��� ���� ��ġ (x, z)�� ���� ������ ����(y ��)�� ���Ѵ�.
	�� ���� ī�޶��� ��ġ ������ y-�� �� �� ũ�� ī�޶� ������ �Ʒ��� �ְ� �ȴ�.
	�̷��� �Ǹ� ���� �׸��� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻� �Ѵ�
	(ī�޶� ���� �ȿ� �����Ƿ� �ﰢ���� ���ε� ������ �ٲ��).
	�̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ �� ���� y-���� �ּҰ���	(������ ���� + 5)�� �����Ѵ�.
	ī�޶��� ��ġ ������ y-���� �ּҰ��� ������ ��� ��ġ���� ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����ؾ� �Ѵ�.*/
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)m_pCameraUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight) {
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA) {
			//3��Ī ī�޶��� ��� ī�޶� ��ġ(y-��ǥ)�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
			ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
			
			//????????????????????????????????????
			XMFLOAT3 xmGetPosition = GetPosition();
			p3rdPersonCamera->SetLookAt(xmGetPosition);
		}
	} 
}
