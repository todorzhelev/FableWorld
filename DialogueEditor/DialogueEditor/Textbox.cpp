#include"Textbox.h"

/*
SourceFile: GuiTextbox.cpp
Purpose: Contains functions for creating and rendering textbox
*/

/*
Function:createGuiTextbox
Purpose: creates the textbox's sprite and texture
*/
void Textbox::Init()
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}

/*
Function:onLostDevice
Purpose:invokes the OnLostDevice function of the sprite
*/
void Textbox::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}

/*
Function:onResetDevice
Purpose:invokes the OnResetDevice function of the sprite
*/
void Textbox::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}

/*
Function:isMouseOver
Purpose:checks if the mouse is over the textbox
*/
bool Textbox::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	ScreenToClient(pApp->GetMainWindow(), &s);

	if((s.x > m_vPosition.x) && 
	   (s.y > m_vPosition.y) &&
	   (s.x< (m_vPosition.x + m_nWidth)) &&
	   (s.y < (m_vPosition.y + m_nHeight)))
	{
		return true;
	}
	else
	{
		return false;
	}
		
}

/*
Function:isMouseDown
Purpose:checks if the mouse button is clicked over the textbox
*/
bool Textbox::IsMouseDown()
{
	if(IsMouseOver() && pDinput->IsMouseButtonDown(0))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void Textbox::OnClicked()
{
	if( IsMouseOver() && m_bIsVisible)
	{
		m_bIsSelected = true;
	}

	if( m_bIsSelected )
	{
		if( pApp->m_strSelectedTextbox.empty() )
		{
			pApp->m_strSelectedTextbox = m_strId;
		}
		else if( m_strId != pApp->m_strSelectedTextbox )
		{
			IBaseMenuObject* pSelectedTextbox = pApp->FindMenuObject(pApp->m_strSelectedTextbox);

			pSelectedTextbox->SetSelected(false);
			pApp->m_strSelectedTextbox = m_strId;
		}
	}
}

/*
Function:onUpdate
Purpose:updates the variables of the guiobject
*/
void Textbox::OnUpdate()
{
	if( IsMouseOver() )
	{
		m_bIsMouseOver = true;
	}
	else 
	{
		m_bIsMouseOver = false;
	}

	if( IsMouseDown() )
	{
		m_bIsMouseDown = true;
	}
	else 
	{
		m_bIsMouseDown = false;
	}
}

/*
Function:onRender
Purpose:renders the textbox's image and text
*/
void Textbox::OnRender()
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if( m_bIsVisible)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

		if( !m_bIsSelected )
		{
			m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
		}
		else
		{
			m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
		}
	}
	m_pSprite->End();

	if( m_bIsVisible )
	{
		pTextManager->RenderText(m_strText.c_str(),m_vPosition.x+10,
			m_vPosition.y+7,
			m_vPosition.x+20,
			m_vPosition.y+32,255,255,255,255);
	}
}


