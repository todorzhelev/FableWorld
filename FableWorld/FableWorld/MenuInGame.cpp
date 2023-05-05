#include <stdafx.h>
#include "MenuInGame.h"

/////////////////////////////////////////////////////////////////////////

MenuInGame::MenuInGame() {

	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);

	m_pLabelContinueGame = new Label(D3DXVECTOR2(posx-40.0f,posy-20.0f),"Continue game");
	m_pLabelContinueGame->SetVisible(true);

	m_pLabelQuit = new Label(D3DXVECTOR2(posx-40.0f,posy+20.0f),"Quit");
	m_pLabelQuit->SetVisible(true);

	OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

void MenuInGame::OnLostDevice() {
	pApp->GetTextManager()->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void MenuInGame::OnResetDevice() {
	pApp->GetTextManager()->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

void MenuInGame::OnUpdate(float dt) {
	float w = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth);
	float h = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight);

	m_pLabelContinueGame->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2-20.0f));
	m_pLabelQuit->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2+20.0f));

	m_pLabelContinueGame->OnUpdate();
	m_pLabelQuit->OnUpdate();

	if (m_pLabelContinueGame->IsMouseDown()) {
		IBaseScene* pGameScene = pApp->GetScene("game");
		pApp->SetCurrentScene(pGameScene);
	}

	if (m_pLabelQuit->IsMouseDown()) {
		PostQuitMessage(0);
	}

	//pApp->GetScene("game")->OnUpdate(dt);
}

/////////////////////////////////////////////////////////////////////////

void MenuInGame::OnRender() {
	pApp->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , 0xff000000, 1.0f, 0);
	pApp->GetDevice()->BeginScene();
	
	pApp->GetScene("game")->OnRender();

	if (!m_pLabelContinueGame->IsMouseOver()) {
		m_pLabelContinueGame->OnRender(255,255,255,255);
	}
	else {
		m_pLabelContinueGame->OnRender(150,255,255,255);
	}

	if (!m_pLabelQuit->IsMouseOver()) {
		m_pLabelQuit->OnRender(255,255,255,255);
	}
	else {
		m_pLabelQuit->OnRender(150,255,255,255);
	}
	
	pApp->GetDevice()->EndScene();
	pApp->GetDevice()->Present(0, 0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////

LRESULT MenuInGame::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_KEYDOWN: {
			switch (wParam) {
				case 'F': {
					pApp->SwitchToFullscreen(true);
				}
				break;
			}
		}
	}

	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////