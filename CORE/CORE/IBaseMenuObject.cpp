#include <stdafx.h>
#include "IBaseMenuObject.h"

/////////////////////////////////////////////////////////////////////////
 
IBaseMenuObject::IBaseMenuObject(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText,std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName, std::string strId)
{
	m_vPosition = vPosition;
	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	m_strText = strText;
	m_strIdleStateTextureFileName	   = strResourcesRelativePath + strIdleStateTexFileName;
	m_strMouseOverStateTextureFileName = strResourcesRelativePath + strMouseOverStateTexFileName;
	m_strId = strId;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetPosition(D3DXVECTOR2 vPosition)
{
	m_vPosition = vPosition;
}

////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR2 IBaseMenuObject::GetPosition()
{
	return m_vPosition;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetMouseOver(bool bIsMouseOver)
{
	m_bIsMouseOver = bIsMouseOver;
}

////////////////////////////////////////////////////////////////////////////////

bool IBaseMenuObject::IsMouseOver()
{
	return m_bIsMouseOver;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetMouseDown(bool bIsMouseDown)
{
	m_bIsMouseDown = bIsMouseDown;
}

////////////////////////////////////////////////////////////////////////////////

bool IBaseMenuObject::IsMouseDown()
{
	return m_bIsMouseDown;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetIdleStateTextureFileName(std::string strFileName)
{
	m_strIdleStateTextureFileName = strFileName;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetMouseOverStateTextureFileName(std::string strFileName)
{
	m_strMouseOverStateTextureFileName = strFileName;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetIdleStateTexture(IDirect3DTexture9* pTexture)
{
	m_pIdleStateTexture = pTexture;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetMouseOverStateTexture(IDirect3DTexture9* pTexture)
{
	m_pMouseOverStateTexture = pTexture;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
}

////////////////////////////////////////////////////////////////////////////////

bool IBaseMenuObject::IsVisible()
{
	return m_bIsVisible;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetText(std::string strText)
{
	m_strText = strText;
}

////////////////////////////////////////////////////////////////////////////////

std::string IBaseMenuObject::GetText()
{
	return m_strText;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

////////////////////////////////////////////////////////////////////////////////

int IBaseMenuObject::GetWidth()
{
	return m_nWidth;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::SetHeight(int nHeight)
{
	m_nHeight = nHeight;
}

////////////////////////////////////////////////////////////////////////////////

int IBaseMenuObject::GetHeight()
{
	return m_nHeight;
}

////////////////////////////////////////////////////////////////////////////////

void IBaseMenuObject::OnClicked()
{


}

////////////////////////////////////////////////////////////////////////////////