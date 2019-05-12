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
	CreateRailTime = (CreateRailTime + 1) % MaxCreateRailTime;
	if (CreateRailTime)	return;

	const int pickrail = m_pointrail;

	// 처음 Rail 만들기
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

	m_pointrail = (m_pointrail + 1) % m_maxrails;
}

void CScene::UpdateBackRail() {
	for (int i = m_pointrail; i < m_pointrail + m_maxrails / 4; ++i) {
		int index = i % m_maxrails;
		if (m_pprail[index]) 
			m_pprail[index]->SetColor(RGB(255, 0, 0));
	}
}

void CScene::UpdateFrontRail(CPlayer * player) {

	CGameObject inputrail;
	inputrail.SetLook(player->GetLook());
	inputrail.SetUp(player->GetUp());
	inputrail.SetRight(player->GetRight());
	inputrail.SetPosition(player->GetPosition());

	XMFLOAT3 railRotate = player->GetRotate();

	for (int i = 0; i < m_maxfrontrails; ++i) {
		if (!m_ppfrontrail[i]) {
			CRailMesh *pObjectRailMesh = new CRailMesh();
			m_ppfrontrail[i] = new CGameObject();
			m_ppfrontrail[i]->SetMesh(pObjectRailMesh);
			m_ppfrontrail[i]->SetColor(RGB(0, 255, 0));
		}

		inputrail.Rotate(railRotate.x * 5, railRotate.y * 5, 0);

		inputrail.SetPosition(Vector3::Add(inputrail.GetPosition(), inputrail.GetLook(), 1.5));

		m_ppfrontrail[i]->SetLook(inputrail.GetLook());
		m_ppfrontrail[i]->SetUp(inputrail.GetUp());
		m_ppfrontrail[i]->SetRight(inputrail.GetRight());

		m_ppfrontrail[i]->SetPosition(inputrail.GetPosition());
	}
}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera, const bool& movetrain){
	if (m_pTerrainObject)
		m_pTerrainObject->Render(hDCFrameBuffer, pCamera);

	if (movetrain) {
		for (int i = 0; i < m_maxfrontrails; i++)
			if (m_ppfrontrail[i])	m_ppfrontrail[i]->Render(hDCFrameBuffer, pCamera);
	}
	for (int i = 0; i < m_maxrails; i++)
		if(m_pprail[i])	m_pprail[i]->Render(hDCFrameBuffer, pCamera);
}

CPlayerScene::CPlayerScene(){
}

CPlayerScene::~CPlayerScene(){
}

void CPlayerScene::BuildObjects(){
	m_maxrails = 100;
	m_pointrail = 0;
	m_pprail = new CGameObject*[m_maxrails];

	for (int i = 0; i < m_maxrails; ++i)
		m_pprail[i] = nullptr;

	m_maxfrontrails = 50;
	m_ppfrontrail = new CGameObject*[m_maxfrontrails];

	for (int i = 0; i < m_maxfrontrails; ++i)
		m_ppfrontrail[i] = nullptr;

	float fHalfWidth = 200.0f, fHalfHeight = 0.0f, fHalfDepth = 200.0f;
	CTerrainMesh *pTerrainMesh = new CTerrainMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 5);
	
	m_pTerrainObject = new CGameObject();
	m_pTerrainObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pTerrainObject->SetMesh(pTerrainMesh);
	m_pTerrainObject->SetColor(RGB(0, 0, 0));

	MaxCreateRailTime = 30;
	CreateRailTime = 0;
}
