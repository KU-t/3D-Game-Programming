#include "stdafx.h"
#include "Scene.h"

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

void CScene::CheckObjectByObjectCollisions(){

}

void CScene::Animate(float fElapsedTime){
	
}

//레일 생성
void CScene::CreateRail(CPlayer * player) {

	const int pickrail = m_pointrail;

	//Rail 만들기
	if (!m_pprail[pickrail]) {
		CRailMesh *pObjectRailMesh = new CRailMesh();
		m_pprail[pickrail] = new CGameObject();
		m_pprail[pickrail]->SetMesh(pObjectRailMesh);
	}

	m_pprail[pickrail]->SetColor(RGB(0, 0, 0));
	m_pprail[pickrail]->SetPosition(player->GetPosition());
	m_pprail[pickrail]->SetLook(player->GetLook());
	m_pprail[pickrail]->SetUp(player->GetUp());
	m_pprail[pickrail]->SetRight(player->GetRight());

	m_pointrail = (m_pointrail + 1) % m_rails;

	UpdateBackRail();
}

void CScene::UpdateBackRail() {
	for (int i = m_pointrail; i < m_pointrail + m_rails / 4; ++i) {
		int index = i % m_rails;
		if (m_pprail[index]) 
			m_pprail[index]->SetColor(RGB(255, 0, 0));
	}
}

void CScene::UpdateFrontRail(CPlayer * player) {
	XMFLOAT3 raillookvector = player->GetLook();
	XMFLOAT3 railupvector = player->GetUp();
	XMFLOAT3 railrightvector = player->GetRight();

	XMFLOAT3 railposition = player->GetPosition();

	for (int i = 0; i < m_frontrails; ++i) {
		if (!m_ppfrontrail[i]) {
			CRailMesh *pObjectRailMesh = new CRailMesh();
			m_ppfrontrail[i] = new CGameObject();
			m_ppfrontrail[i]->SetMesh(pObjectRailMesh);
			m_ppfrontrail[i]->SetColor(RGB(0, 255, 0));
		}

		m_ppfrontrail[i]->SetLook(raillookvector);
		m_ppfrontrail[i]->SetUp(railupvector);
		m_ppfrontrail[i]->SetRight(railrightvector);

		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, raillookvector, 1.5);
		railposition = Vector3::Add(xmf3Shift, railposition);

		m_ppfrontrail[i]->SetPosition(railposition);
	}
}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera, const bool& movetrain){
	if (m_pTerrainObject)
		m_pTerrainObject->Render(hDCFrameBuffer, pCamera);

	if (movetrain) {
		for (int i = 0; i < m_frontrails; i++)
			if (m_ppfrontrail[i])	m_ppfrontrail[i]->Render(hDCFrameBuffer, pCamera);
	}
	for (int i = 0; i < m_rails; i++)
		if(m_pprail[i])	m_pprail[i]->Render(hDCFrameBuffer, pCamera);
}

CPlayerScene::CPlayerScene(){
}

CPlayerScene::~CPlayerScene(){
}

void CPlayerScene::BuildObjects(){
	m_rails = 100;
	m_pprail = new CGameObject*[m_rails];

	for (int i = 0; i < m_rails; ++i)
		m_pprail[i] = nullptr;

	m_frontrails = 8;
	m_ppfrontrail = new CGameObject*[m_frontrails];

	for (int i = 0; i < m_frontrails; ++i)
		m_ppfrontrail[i] = nullptr;

	float fHalfWidth = 100.0f, fHalfHeight = 0.0f, fHalfDepth = 100.0f;
	CTerrainMesh *pTerrainMesh = new CTerrainMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 5);
	
	m_pTerrainObject = new CGameObject();
	m_pTerrainObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pTerrainObject->SetMesh(pTerrainMesh);
	m_pTerrainObject->SetColor(RGB(0, 0, 0));
}
