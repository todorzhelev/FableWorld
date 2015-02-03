#include "IBase2DMenuObject.h"


IBase2DMenuObject::IBase2DMenuObject(string strText, bool bIsVisible, int nWidth, int nHeight, D3DXVECTOR2 vPos, bool bIsMouseDown, bool bIsMouseOver)
	:IBaseMenuObject(strText, bIsVisible,nWidth,nHeight,vPos,bIsMouseDown,bIsMouseOver)
{
}