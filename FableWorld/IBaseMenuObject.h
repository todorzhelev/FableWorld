#pragma once
#include"Misc.h"

class IBaseMenuObject
{
public:

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;
	
	virtual bool IsMouseOver() = 0;

	virtual bool IsMouseDown() = 0;

	virtual void OnUpdate() = 0;

	virtual void OnRender() {};

	virtual bool IsClicked() { return false; }

	void		 SetPosition(D3DXVECTOR2 vPosition);

	D3DXVECTOR2  GetPosition();

	void		 SetMouseOver(bool bIsMouseOver);

	bool		 GetIsMouseOver();

	void		 SetMouseDown(bool bIsMouseDown);

	bool		 GetIsMouseDown();

	void		 SetVisible(bool bVisible);

	bool		 IsVisible();

	void		 SetIdleStateTextureFileName(string strFileName);

	void		 SetMouseOverStateTextureFileName(string strFileName);

	void		 SetIdleStateTexture(IDirect3DTexture9* pTexture);

	void		 SetMouseOverStateTexture(IDirect3DTexture9* pTexture);

	void		 SetText(string strText);

	string		 GetText();

	void		 SetWidth(int nWidth);

	int			 GetWidth();

	void		 SetHeight(int nHeight);

	int			 GetHeight();

protected:

	D3DXVECTOR2 m_vPosition;

	ID3DXSprite* m_pSprite;

	int m_nWidth;
	int m_nHeight;

	bool m_bIsMouseOver;
	bool m_bIsMouseDown;

	bool m_bIsVisible;

	string m_strText;

	string m_strIdleStateTextureFileName;
	string m_strMouseOverStateTextureFileName;

	IDirect3DTexture9* m_pIdleStateTexture;
	IDirect3DTexture9* m_pMouseOverStateTexture;
};

