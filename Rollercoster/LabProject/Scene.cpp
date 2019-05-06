#include "stdafx.h"
#include "Scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::BuildObjects()
{
}

void CScene::ReleaseObjects()
{
}

//사각형 충돌체크
void CScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}
}

void CScene::Animate(float fElapsedTime)
{
	// 사각형 돌리기
	//for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fElapsedTime);

	//CheckObjectByObjectCollisions();

	
}

//레일 생성
void CScene::CreateRail(CPlayer * player) {

	//Rail 만들기
	CRailMesh *pObjectRailMesh = new CRailMesh();

	const int pickrail = m_pointrail;

	if (!m_pprail[pickrail]) {
		m_pprail[pickrail] = new CGameObject();
		m_pprail[pickrail]->SetMesh(pObjectRailMesh);
		m_pprail[pickrail]->SetColor(RGB(185, 122, 87));

		m_pprail[pickrail]->SetPosition(player->GetPosition());
		m_pprail[pickrail]->SetLook(player->GetLook());
		m_pprail[pickrail]->SetUp(player->GetUp());
		m_pprail[pickrail]->SetRight(player->GetRight());
	}

	else {
		m_pprail[pickrail]->SetPosition(player->GetPosition());
		m_pprail[pickrail]->SetLook(player->GetLook());
		m_pprail[pickrail]->SetUp(player->GetUp());
		m_pprail[pickrail]->SetRight(player->GetRight());
	}

	m_pointrail = (m_pointrail + 1) % m_rails;
}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_rails; i++)
		if(m_pprail[i])	m_pprail[i]->Render(hDCFrameBuffer, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerScene::CPlayerScene()
{
}

CPlayerScene::~CPlayerScene()
{
}

void CPlayerScene::BuildObjects()
{
	m_rails = 50;
	m_pprail = new CGameObject*[m_rails];

	for (int i = 0; i < m_rails; ++i)
		m_pprail[i] = nullptr;
}
