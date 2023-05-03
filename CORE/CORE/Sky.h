#pragma once
#include "Camera.h"
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

class Sky {
public:
	Sky(std::string strSkyTextureFileName, float fSkyRadius);
	~Sky();
	void OnLostDevice();
	void OnResetDevice();
	void OnRender(const std::unique_ptr<Camera>& camera);
private:
	ID3DXMesh*	m_pSkyMesh;
	float		m_fSkyRadius;
	IDirect3DCubeTexture9* m_pSkyTexture;
	ID3DXEffect*	m_pEffect;
	D3DXHANDLE		m_hEffectTechnique;
	D3DXHANDLE		m_hSkyTexture;
	D3DXHANDLE		m_hWVPMatrix;
};

//////////////////////////////////////////////////////////////////////////////
