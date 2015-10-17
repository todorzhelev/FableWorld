#include "MenuInGame.h"
/*
SourceFile:MenuInGame.cpp
Purpose:loads the labels for the menu in game and renders them
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:MenuInGame
Purpose:constructor. It loads all the functionality in the menu in game
*/
MenuInGame::MenuInGame()
{

	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);

	m_pLabelContinueGame = new Label(D3DXVECTOR2(posx-40.0f,posy-20.0f),"Continue game");
	m_pLabelContinueGame->SetVisible(true);

	m_pLabelQuit = new Label(D3DXVECTOR2(posx-40.0f,posy+20.0f),"Quit");
	m_pLabelQuit->SetVisible(true);

	OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void MenuInGame::OnLostDevice()
{
	pTextManager->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invokes the corresponding onResetDevice functions
*/
void MenuInGame::OnResetDevice()
{
	pTextManager->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate
Purpose:updates the labels in the menu in game
*/
void MenuInGame::OnUpdate(float dt)
{
	pDinput->Poll();
	if( pDinput->IsKeyDown(DIK_F))
	{
		pApp->SwitchToFullscreen(true);
	}
	float w = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth);
	float h = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight);

	m_pLabelContinueGame->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2-20.0f));
	m_pLabelQuit->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2+20.0f));

	m_pLabelContinueGame->OnUpdate();
	m_pLabelQuit->OnUpdate();

	if(m_pLabelContinueGame->IsMouseDown())
	{
		IBaseScene* pGameScene = pApp->GetScene("game");
		pApp->SetCurrentScene(pGameScene);
	}

	if(m_pLabelQuit->IsMouseDown())
	{
		PostQuitMessage(0);
	}

	//pApp->GetScene("game")->OnUpdate(dt);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onRender
Purpose:render all
*/
void MenuInGame::OnRender()
{
	pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , 0xff000000, 1.0f, 0);
	pDxDevice->BeginScene();
	
	pApp->GetScene("game")->OnRender();

	if(!m_pLabelContinueGame->IsMouseOver())
	{
		m_pLabelContinueGame->OnRender(255,255,255,255);
	}
	else
	{
		m_pLabelContinueGame->OnRender(150,255,255,255);
	}

	if(!m_pLabelQuit->IsMouseOver())
	{
		m_pLabelQuit->OnRender(255,255,255,255);
	}
	else
	{
		m_pLabelQuit->OnRender(150,255,255,255);
	}
	
	pDxDevice->EndScene();
	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:MenuInGame
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching.
		Isnt used here, because DirectInput works fine for the things we need in menu in game
*/
LRESULT MenuInGame::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{

	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////

	
