#include"Label.h"
#include"TextManager.h"
/*
SourceFile: GuiLabel.cpp
Purpose: Contains functions for loading and rendering labels
*/

Label::Label(D3DXVECTOR2 vPosition, string strText)
{
	m_vPosition = vPosition;
	m_strText = strText;

	m_nHeight = static_cast<int>(GetStringHeight(m_strText));
	m_nWidth  = static_cast<int>(GetStringWidth(m_strText));

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:Init
Purpose:sets the width and the height of the label
*/
void Label::Init()
{
	m_nHeight = static_cast<int>(GetStringHeight(m_strText));
	m_nWidth  = static_cast<int>(GetStringWidth(m_strText));

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:invokes the OnLostDevice function of the sprite 
*/
void Label::OnLostDevice()
{
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:invokes the OnResetDevice function of the sprite 
*/
void Label::OnResetDevice()
{
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseOver
Purpose:checks if mouse is over tha label
*/
bool Label::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	//ScreenToClient makes the mouse coordinates relative to area of the program not to the screen 
	//i.e. the coordinates will start from top-left position of the window
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
Purpose:if mouse is over the label and mouse buttons is clicked and the label isnt hidden it returns true
*/
bool Label::IsMouseDown()
{
	if(IsMouseOver() && pDinput->IsMouseButtonDown(0) && m_bIsVisible)
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
void Label::OnUpdate()
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
Purpose:renders the label
*/
void Label::OnRender(int a,int r,int g,int b)
{
	if( m_bIsVisible )
	{
		pTextManager->RenderText(m_strText.c_str(),
							     static_cast<int>(m_vPosition.x),
							     static_cast<int>(m_vPosition.y),
							     static_cast<int>(m_vPosition.x)+m_nWidth,
							     static_cast<int>(m_vPosition.y)+m_nHeight,
							     a,r,g,b);
	}
}	


/////////////////////////////////////////////////////////////////////////
