#pragma once
#include "Camera.h"
#include "Misc.h"
#include "AABB.h"

//////////////////////////////////////////////////////////////////////////////

const int k_nSubGridsRowsNumber = 65;
const int k_nSubGridsColsNumber = 65;
const int k_nSubGridsTrianglesNumber  = (k_nSubGridsRowsNumber-1) * (k_nSubGridsColsNumber-1) * 2;
const int k_nSubGridsVertsNumber	  = k_nSubGridsRowsNumber * k_nSubGridsColsNumber;

//////////////////////////////////////////////////////////////////////////////

struct TerrainSubGrid
{
	TerrainSubGrid(ID3DXMesh* mesh, AABB bb)
	{
		m_pSubGridMesh = mesh;
		m_subGridBoungingBox = bb;
	}

	ID3DXMesh*	m_pSubGridMesh;
	AABB		m_subGridBoungingBox;
};

//////////////////////////////////////////////////////////////////////////////

class Terrain
{
public:

	Terrain(string strHeightmapFileName,float fHeightsScale, int nRows,int nCols,float fDX,float fDZ ,const D3DXVECTOR3& vCenterPointr);

	void	OnRender();

	void	OnLostDevice();

	void	OnResetDevice();

	float	GetHeight(float fX, float fZ);

	bool	IsValidPosition(float x, float z);

	bool	IsInBounds(int i, int j);

	void	SetLightVector(D3DXVECTOR3 vLightVector);

	void	LoadHeightmap();

	void	GenerateTerrainMesh();

	void	GenerateVertexBuffer(vector<D3DXVECTOR3>& vVertices, int nNumRows,int nNumCols);

	void	GenerateIndexBuffer(vector<DWORD>& vIndices,int nNumRows,int nNumCols);

	void	BuildEffect();

	void	BuildSubGridMesh(RECT& rSubGridRectangle, VertexPositionNormalTexture* pVertices); 

private:

	string	m_strHeightmapFileName;

	//modifies the heights by some value
	float	m_fHeightsScale;
	int		m_nRows;
	int		m_nCols;
	int		m_nNumVertices;
	int		m_nNumIndices;
	int		m_nNumTriangles;
	//distance between two vertices in the terrain across the x axis
	float	m_fDX;
	//distance between two vertices in the terrain across the z axis
	float	m_fDZ;	
	int		m_nWidth;
	int		m_nDepth;
	D3DXVECTOR3	m_vCenterPoint;

	//vector containing the height of every vertex in the terrain
	vector<float> m_vHeightmap;

	//vector with subgrids of the terrain
	vector<TerrainSubGrid*> m_vSubGrids;

	IDirect3DTexture9* m_pBlendMapTexture;
	IDirect3DTexture9* m_pStoneTexture;
	IDirect3DTexture9* m_pDirtTexture;
	IDirect3DTexture9* m_pGrassTexture;

	//variables needed for the shader
	ID3DXEffect* m_pEffect;
	D3DXHANDLE   m_hEffectTechnique;
	D3DXHANDLE   m_hWVPMatrix;
	D3DXHANDLE   m_hBlendMap;
	D3DXHANDLE   m_hStoneTexture;
	D3DXHANDLE   m_hDirtTexture;
	D3DXHANDLE   m_hGrassTexture;
	D3DXHANDLE   m_hLightVector;
};

//////////////////////////////////////////////////////////////////////////////

extern Terrain* pTerrain;

//////////////////////////////////////////////////////////////////////////////