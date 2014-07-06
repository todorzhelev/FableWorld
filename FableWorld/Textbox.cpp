#include"Textbox.h"
#include"TextManager.h"
/*
SourceFile: GuiTextbox.cpp
Purpose: Contains functions for creating and rendering textbox. Textbox is currently not used by the game, it is used in the level editor.
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Init
Purpose: creates the textbox's sprite and texture
*/
void Textbox::Init()
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:invokes the OnLostDevice function of the sprite
*/
void Textbox::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:invokes the OnResetDevice function of the sprite
*/
void Textbox::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseOver
Purpose:checks if the mouse is over the textbox
*/
bool Textbox::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	ScreenToClient(pEngine->GetMainWindow(), &s);

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


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseDown
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


/////////////////////////////////////////////////////////////////////////
/*
Function:OnUpdate
Purpose:updates the variables of the guiobject
*/
void Textbox::OnUpdate()
{
	if(IsMouseOver())
	{
		m_bIsMouseOver = true;
	}
	else 
	{
		m_bIsMouseOver = false;
	}

	if(IsMouseDown())
	{
		m_bIsMouseDown = true;
	}
	else 
	{
		m_bIsMouseDown = false;
	}

}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:renders the textbox's image and text
*/
void Textbox::OnRender()
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if(m_bIsVisible)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	m_pSprite->End();
		
	if(m_bIsVisible)
	{
		pTextManager->RenderText(m_strText.c_str(),
							     static_cast<int>(m_vPosition.x)+10,
							     static_cast<int>(m_vPosition.y)+7,
							     static_cast<int>(m_vPosition.x)+20,
							     static_cast<int>(m_vPosition.y)+32,
							     255,255,255,255);
	}
}

/////////////////////////////////////////////////////////////////////////

