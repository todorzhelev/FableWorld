#include"IBase3DMenuObject.h"


void IBase3DMenuObject::InitShader()
{
	D3DXCreateEffectFromFile(pDxDevice, "shaders/DebugGraphicsShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pShaderEffect, 0);
	m_hShaderTechnique  = m_pShaderEffect->GetTechniqueByName("DebugGraphics3DTech");
	m_hShaderWVPMatrix  = m_pShaderEffect->GetParameterByName(0, "WVP");
}