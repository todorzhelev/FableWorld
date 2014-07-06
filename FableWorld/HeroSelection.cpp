#include "HeroSelection.h"
#include "Game.h"
#include "TextManager.h"

/*
SourceFile:HeroSelection.cpp
Purpose:loads the logo and labels for the menu and renders them
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Menu
Purpose:constructor. It loads all the functionality in the menu
*/
HeroSelection::HeroSelection()
{
	camera->GetPosition() = D3DXVECTOR3(-2,31,-160);

	GameObject* pMesh = new SkinnedMesh("pesho","models/ezreal/ezreal_5anm.x","textures/Ezreal_ProdigalExplorer.dds",false);

	GameObject* pLand = new StaticMesh("land","models/land.x","textures/grass-texture-02.dds");

	pMesh->LoadGameObject();
	pLand->LoadGameObject();

	//pEngine->AddGameObject(pMesh);
	//pEngine->AddGameObject(pLand);

	m_SceneObjects.push_back(pMesh);
	m_SceneObjects.push_back(pLand);
	
	pTextManager->CreateMeshFor3DText(pMesh);

	pPesho = pMesh;

	float posx = static_cast<float>(pEngine->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pEngine->GetPresentParameters().BackBufferHeight/2);

	m_pLabelEnterWorld = new Label;
	m_pLabelEnterWorld->SetText("Enter World");
	m_pLabelEnterWorld->SetVisible(true);
	m_pLabelEnterWorld->SetHeight(static_cast<int>(GetStringHeight(m_pLabelEnterWorld->GetText())));
	m_pLabelEnterWorld->SetWidth(static_cast<int>(GetStringWidth(m_pLabelEnterWorld->GetText())));
	m_pLabelEnterWorld->SetMouseDown(false);
	m_pLabelEnterWorld->SetMouseOver(false) ;
	m_pLabelEnterWorld->SetPosition(D3DXVECTOR2(posx-m_pLabelEnterWorld->GetWidth()/2,2*posy - 75));

}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void HeroSelection::OnLostDevice()
{
	m_pLabelEnterWorld->OnLostDevice();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnLostDevice();
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invokes the corresponding OnResetDevice functions
*/
void HeroSelection::OnResetDevice()
{
	m_pLabelEnterWorld->OnResetDevice();

	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnResetDevice();
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate
Purpose:updates all the functionallity in menu
*/
void HeroSelection::OnUpdate(float dt)
{
	pDinput->Poll();

	if(m_pLabelEnterWorld->IsMouseDown() )
	{
		pEngine->GetScene("heroSelect")->OnLostDevice();

		IBaseScene* pGameScene = pEngine->GetScene("game");
		pEngine->SetCurrentScene(pGameScene);
	}

	if(pDinput->IsMouseButtonDown(0) )
	{
		SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>(pPesho);
	
		float yAngle = pDinput->GetMouseDX() / (30000*dt);
		pSkinnedMesh->m_fRotAngleY -=yAngle;
	
		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, yAngle);
		D3DXVec3TransformCoord(&pSkinnedMesh->m_vRight, &pSkinnedMesh->m_vRight, &R);
		D3DXVec3TransformCoord(&pSkinnedMesh->m_vUp , &pSkinnedMesh->m_vUp , &R);
		D3DXVec3TransformCoord(&pSkinnedMesh->m_vLook, &pSkinnedMesh->m_vLook, &R);
	}

	camera->OnUpdate(dt);
	//camera->MoveCamera(dt);
	for( GameObject* pObject : m_SceneObjects )
	{
		pObject->OnUpdate(dt);
	}

	m_pLabelEnterWorld->OnUpdate();
			
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onRender
Purpose:render all
*/
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
		m_pLabelEnterWorld->OnRender(255,255,255,255);
	}
	else
	{
		m_pLabelEnterWorld->OnRender(150,255,255,255);
	}

	pDxDevice->EndScene();
	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:msgProc
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching.Isnt used here, because DirectInput works fine for the things we need in menu
*/
LRESULT HeroSelection::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	return DefWindowProc(pEngine->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////


	
