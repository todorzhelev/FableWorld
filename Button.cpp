#include"Button.h"

/*
SourceFile: GuiButton.cpp
Purpose: Contains functions for loading and rendering buttons
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Init
Purpose:creates to sprite for rendering the button and loads the textures of the button:
		the mouseover texture and normal texture
*/
void Button::Init()
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);
	m_bIsMouseOver = true;
	m_bIsMouseDown = true;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:invokes the OnLostDevice function of the sprite 
*/
void Button::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:invokes the OnResetDevice function of the sprite 
*/
void Button::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseOver
Purpose:checks if the mouse is over the texture
*/
bool Button::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	//ScreenToClient makes the mouse coordinates relative to area of the program not to the screen 
	//i.e. the coordinates will start from top-left position of the window
	ScreenToClient(pEngine->GetMainWindow(), &s);

	//if the mouse is inside return true
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
Purpose:if the mouse is over the image and the mouse button is clicked we got mouse down event
*/
bool Button::IsMouseDown()
{
	//if the mouse is over the area and the left button is pressed then return true
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
Function:IsClicked
Purpose:if the button is clicked
*/
bool Button::IsClicked()
{
	return m_bIsMouseDown;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnUpdate
Purpose:updates the variables of the guiobject
*/
void Button::OnUpdate()
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
Purpose:render the button
*/
void Button::OnRender()
{
	D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//if the mouse is over the button draw the mouseOverText
	if(m_bIsMouseOver)
	{
		m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	else
	{
		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	m_pSprite->End();
}


/////////////////////////////////////////////////////////////////////////