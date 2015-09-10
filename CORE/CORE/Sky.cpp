#include "Sky.h"

/////////////////////////////////////////////////////////////////////////
/*
Function:Sky
Purpose:constructor
*/
Sky::Sky(string strSkyTextureFileName, float fSkyRadius)
	:m_fSkyRadius(fSkyRadius)
{
	D3DXCreateSphere(pDxDevice, m_fSkyRadius, 160, 160, &m_pSkyMesh, 0);
	D3DXCreateCubeTextureFromFile(pDxDevice, strSkyTextureFileName.c_str(), &m_pSkyTexture);

	D3DXCreateEffectFromFile(pDxDevice, "../../CORE/CORE/shaders/SkyShader.fx", 0, 0, 0, 0, &m_pEffect, 0);

	m_hEffectTechnique	= m_pEffect->GetTechniqueByName("SkyTech");
	m_hWVPMatrix		= m_pEffect->GetParameterByName(0, "WVP");
	m_hSkyTexture		= m_pEffect->GetParameterByName(0, "tex0");

	m_pEffect->SetTechnique(m_hEffectTechnique);
	m_pEffect->SetTexture(m_hSkyTexture, m_pSkyTexture);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:~Sky
Purpose:destructor
*/
Sky::~Sky()
{
	releaseX(m_pSkyMesh);
	releaseX(m_pSkyTexture);
	releaseX(m_pEffect);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:
*/
void Sky::OnLostDevice()
{
	m_pEffect->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:
*/
void Sky::OnResetDevice()
{
	m_pEffect->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:renders the sky
*/
void Sky::OnRender()
{
	//Translate the sky mesh to the camera position, this way we will never reach the end of the sky
	D3DXMATRIX TranslationMatrix;
	D3DXVECTOR3 vCameraPos = camera->GetPosition();
	D3DXMatrixTranslation(&TranslationMatrix, vCameraPos.x, vCameraPos.y, vCameraPos.z);
	m_pEffect->SetMatrix(m_hWVPMatrix, &(TranslationMatrix * camera->GetViewProjMatrix()));
	
	UINT nNumPasses = 0;
	m_pEffect->Begin(&nNumPasses, 0);
	m_pEffect->BeginPass(0);

		m_pSkyMesh->DrawSubset(0);

	m_pEffect->EndPass();
	m_pEffect->End();
}


/////////////////////////////////////////////////////////////////////////