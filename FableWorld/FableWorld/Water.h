#pragma once
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

class Water {
public:
	struct InitInfo {
		Light	 dirLight;
		Material mtrl;
		int      vertRows;
		int      vertCols;
		float    dx;
		float    dz;
		std::string waveMapFilename0;
		std::string waveMapFilename1;
		std::string dmapFilename0;
		std::string dmapFilename1;
		D3DXVECTOR2 waveNMapVelocity0;
		D3DXVECTOR2 waveNMapVelocity1;
		D3DXVECTOR2 waveDMapVelocity0;
		D3DXVECTOR2 waveDMapVelocity1;
		D3DXVECTOR2 scaleHeights;
		float texScale;
		D3DXMATRIX toWorld;
	};
public:
	Water(InitInfo& initInfo);
	~Water();
	void OnLostDevice();
	void OnResetDevice();
	void OnUpdate(float dt);
	void OnRender(const std::unique_ptr<Camera>& camera);
	void SetEnvMap(IDirect3DCubeTexture9* envMap);
private:
	void buildFX();
	void GenTriGrid(int numVertRows, int numVertCols, float dx, float dz, const D3DXVECTOR3& center, std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices);

private:
	std::unique_ptr<ID3DXMesh> m_pMesh;
	std::unique_ptr<ID3DXEffect> m_pFX;

	// The two normal maps to scroll.
	std::unique_ptr<IDirect3DTexture9> m_pWaveMap0;
	std::unique_ptr<IDirect3DTexture9> m_pWaveMap1;

	// The two displacement maps to scroll.
	std::unique_ptr<IDirect3DTexture9> m_pDispMap0;
	std::unique_ptr<IDirect3DTexture9> m_pDispMap1;

	// Offset of normal maps for scrolling (vary as a function of time)
	D3DXVECTOR2 m_waveNMapOffset0;
	D3DXVECTOR2 m_waveNMapOffset1;

	// Offset of displacement maps for scrolling (vary as a function of time)
	D3DXVECTOR2 m_waveDMapOffset0;
	D3DXVECTOR2 m_waveDMapOffset1;

	InitInfo m_initInfo;
	float m_width;
	float m_depth;

	D3DXHANDLE m_hTech;
	D3DXHANDLE m_hWVP;
	D3DXHANDLE m_hWorld;
	D3DXHANDLE m_hWorldInv;
	D3DXHANDLE m_hLight;
	D3DXHANDLE m_hMtrl;
	D3DXHANDLE m_hEyePosW;
	D3DXHANDLE m_hWaveMap0;
	D3DXHANDLE m_hWaveMap1;
	D3DXHANDLE m_hWaveNMapOffset0;
	D3DXHANDLE m_hWaveNMapOffset1;
	D3DXHANDLE m_hWaveDMapOffset0;
	D3DXHANDLE m_hWaveDMapOffset1;
	D3DXHANDLE m_hWaveDispMap0;
	D3DXHANDLE m_hWaveDispMap1;
	D3DXHANDLE m_hScaleHeights;
	D3DXHANDLE m_hGridStepSizeL;
	D3DXHANDLE m_hEnvMap;
};

//////////////////////////////////////////////////////////////////////////////