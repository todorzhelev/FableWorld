#include <stdafx.h>
#include "HeroSelection.h"
#include "Game.h"
#include "../../CORE/CORE/TextManager.h"

//TODO: move this from here...
constexpr int rotationSlowFactor = 20000;

/////////////////////////////////////////////////////////////////////////

HeroSelection::HeroSelection()
{
	//TODO:this should be specified in the level file
	camera->GetPosition() = D3DXVECTOR3(-100, 31, -5);

	//TODO: use level file, instead of hardcoded stuff
	//luaL_dofile(g_luaState, "../../Resources/scripts/levelHeroSelect.lua");

	//SkinnedModel* pMesh = new SkinnedModel("Ezrael","../../Resources/models/ezreal/ezreal_5anm.x","../../Resources/textures/SkinnedModels/Ezreal_ProdigalExplorer.dds",false);
	SkinnedModel* pMesh = new SkinnedModel("cho", "../../Resources/models/cho/cho_5anmx.X", "../../Resources/textures/SkinnedModels/chogath_abyss_TX_CM.DDS", false);

	GameObject* pLand = new StaticModel("land","../../Resources/models/land.x","../../Resources/textures/Terrain/grass-texture-02.dds");

	pSky = new Sky("../../Resources/textures/Sky/grassenvmap1024.dds", 10000.0f);

	pMesh->LoadGameObject();
	pLand->LoadGameObject();

	//pLand->SetScale(1.3); 
	pMesh->SetScale(0.2);
	pMesh->SetRotationAngleByY(D3DX_PI / 2);
	pMesh->SetTitleRotationAnglyByY(D3DX_PI / 2);

	m_SceneObjects.push_back(pMesh);
	m_SceneObjects.push_back(pLand);
	
	pTextManager->CreateMeshFor3DText(pMesh);

	m_pMainHero = pMesh;

	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);
	int width = GetStringWidth("Enter world");

	m_pLabelEnterWorld = new Label(D3DXVECTOR2(posx-width/2-20,2*posy - 75),"Enter world");
	m_pLabelEnterWorld->SetVisible(true);
}

/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnLostDevice()
{
	pSky->OnLostDevice();
	m_pLabelEnterWorld->OnLostDevice();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnLostDevice();
	}
}


/////////////////////////////////////////////////////////////////////////

void HeroSelection::OnResetDevice()
{
	pSky->OnResetDevice();
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
		if (m_pMainHero)
		{
			SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(m_pMainHero);

			float yAngle = pDinput->GetMouseDX() / (rotationSlowFactor*dt);
			pSkinnedModel->ModifyRotationAngleByY(-yAngle);

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, yAngle);
			D3DXVec3TransformCoord(&pSkinnedModel->GetRightVector(), &pSkinnedModel->GetRightVector(), &R);
			D3DXVec3TransformCoord(&pSkinnedModel->GetUpVector(), &pSkinnedModel->GetUpVector(), &R);
			D3DXVec3TransformCoord(&pSkinnedModel->GetLookVector(), &pSkinnedModel->GetLookVector(), &R);
		}
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

	pSky->OnRender();
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


	
