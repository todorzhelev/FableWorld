#include "HeroSelection.h"
#include "Game.h"
#include "../../CORE/CORE/TextManager.h"

constexpr int rotationSlowFactor = 200000;

/////////////////////////////////////////////////////////////////////////

HeroSelection::HeroSelection()
{
	camera->GetPosition() = D3DXVECTOR3(-2,31,-160);

	SkinnedModel* pMesh = new SkinnedModel("Ezrael","../../Resources/models/ezreal/ezreal_5anm.x","../../Resources/textures/SkinnedModels/Ezreal_ProdigalExplorer.dds",false);

	GameObject* pLand = new StaticModel("land","../../Resources/models/land.x","../../Resources/textures/Terrain/grass-texture-02.dds");

	pMesh->LoadGameObject();
	pLand->LoadGameObject();

	m_SceneObjects.push_back(pMesh);
	m_SceneObjects.push_back(pLand);
	
	pTextManager->CreateMeshFor3DText(pMesh);

	pPesho = pMesh;

	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);
	int width = GetStringWidth("Enter world");

	m_pLabelEnterWorld = new Label(D3DXVECTOR2(posx-width/2,2*posy - 75),"Enter world");
	m_pLabelEnterWorld->SetVisible(true);
}

/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnLostDevice()
{
	m_pLabelEnterWorld->OnLostDevice();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnLostDevice();
	}
}


/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnResetDevice()
{
	m_pLabelEnterWorld->OnResetDevice();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnResetDevice();
	}
}


/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnUpdate(float dt)
{
	pDinput->Poll();

	if(m_pLabelEnterWorld->IsMouseDown() )
	{
		pApp->GetScene("heroSelect")->OnLostDevice();

		IBaseScene* pGameScene = pApp->GetScene("game");
		pApp->SetCurrentScene(pGameScene);
	}

	if(pDinput->IsMouseButtonDown(0) )
	{
		SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(pPesho);
	
		float yAngle = pDinput->GetMouseDX() / (rotationSlowFactor*dt);
		pSkinnedModel->ModifyRotationAngleByY(-yAngle);
	
		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, yAngle);
		D3DXVec3TransformCoord(&pSkinnedModel->GetRightVector(), &pSkinnedModel->GetRightVector(), &R);
		D3DXVec3TransformCoord(&pSkinnedModel->GetUpVector() , &pSkinnedModel->GetUpVector() , &R);
		D3DXVec3TransformCoord(&pSkinnedModel->GetLookVector(), &pSkinnedModel->GetLookVector(), &R);
	}

	camera->OnUpdate(dt);
	//camera->MoveCamera(dt);
	for( auto& pObject : m_SceneObjects )
	{
		pObject->OnUpdate(dt);
	}

	m_pLabelEnterWorld->OnUpdate();
			
}


/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnRender()
{
	pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	pDxDevice->BeginScene();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnRender();
	}
		
	if(!m_pLabelEnterWorld->IsMouseOver())
	{
		m_pLabelEnterWorld->OnRender(255,0,0,0);
	}
	else
	{
		m_pLabelEnterWorld->OnRender(150,255,255,255);
	}

	pDxDevice->EndScene();
	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////

LRESULT HeroSelection::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////


	
