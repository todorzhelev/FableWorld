#pragma once
#include "Camera.h"
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

class Sky
{
public:

	Sky(string strSkyTextureFileName, float fSkyRadius);

	~Sky();

	float GetRadius();

	void OnLostDevice();

	void OnResetDevice();

	void OnRender();

private:

	ID3DXMesh*	m_pSkyMesh;
	float		m_fSkyRadius;
	IDirect3DCubeTexture9* m_pSkyTexture;

	//variables needed for the shader
	ID3DXEffect*	m_pEffect;
	D3DXHANDLE		m_hEffectTechnique;
	D3DXHANDLE		m_hSkyTexture;
	D3DXHANDLE		m_hWVPMatrix;
};

//////////////////////////////////////////////////////////////////////////////
