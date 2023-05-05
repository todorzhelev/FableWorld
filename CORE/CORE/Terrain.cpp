#include <stdafx.h>
#include "Terrain.h"
#include "Camera.h"

/////////////////////////////////////////////////////////////////////////
/*
		   +z
	----------------
	|				|
	|				|
-x	|		.		| +x
	|				|
	|				|
	----------------
			-z

if we have 512x512 logical map with 1 logical meter, we will have
map with coordinates ranging from -256,256 (left up corner) to 256,-256 (down right corner)

physically it feels that 1 logical meter is around 0.03 physical meters
and 1 physical meter is 35 logical meters
in this sense 512x512 logical map feels like 15x15 physical map or 225m^2

513x513 = 263169 vertices, 32B per vertex, which results in 8,5MB for the terrain
5130x5130 => 840MB for the terrain

*/

/////////////////////////////////////////////////////////////////////////

//TODO: do we need to translate the terrain to the center of the coordinate system
// and after that return it back to fourth quadrant?

/////////////////////////////////////////////////////////////////////////

//First the heightmap is loaded,then based on loaded values from heightmap and rows,cols,dx,dz large terrain mesh is generated.
Terrain::Terrain(std::string strHeightmapFileName,float fHeightsScale, int nRows,int nCols,float fDX,float fDZ ,const D3DXVECTOR3& vCenterPoint)
:m_strHeightmapFileName(strHeightmapFileName)
,m_fHeightsScale(fHeightsScale)
,m_nRows(nRows)
,m_nCols(nCols)
,m_fDX(fDX)
,m_fDZ(fDZ)
,m_vCenterPoint(vCenterPoint) {
	m_nNumVertices	= m_nRows * m_nCols;
	m_nNumTriangles = (m_nRows-1) * (m_nCols-1) * 2;
	m_nNumIndices	= (m_nRows-1) * (m_nCols-1) * 6;
	m_nWidth		= static_cast<int>((m_nCols-1) * m_fDX);
	m_nDepth		= static_cast<int>((m_nRows-1) * m_fDZ);

	LoadHeightmap();

	CheckSuccess(D3DXCreateTextureFromFile(pApp->GetDevice(), "../../Resources/textures/Terrain/BlendMap_new.dds", &m_pBlendMapTexture));
	CheckSuccess(D3DXCreateTextureFromFile(pApp->GetDevice(), "../../Resources/textures/Terrain/ground0.dds", &m_pDirtTexture));
	CheckSuccess(D3DXCreateTextureFromFile(pApp->GetDevice(), "../../Resources/textures/Terrain/grass-texture-02.dds", &m_pGrassTexture));
	CheckSuccess(D3DXCreateTextureFromFile(pApp->GetDevice(), "../../Resources/textures/Terrain/rocks.dds", &m_pStoneTexture));

	GenerateTerrainMesh();
	BuildEffect();	
}

/////////////////////////////////////////////////////////////////////////
/*
LoadHeightmap() opens the passed file for heightmap and read its values in binary mode.
	  This values are used as heights for the terrain.For heightmap can be used normal picture .jpg .bmp .raw.
	  in the file values are not stored as 64x64 matrix(for instance) which we need to simulate 64x64 terrain,but like this
	  ¨§££ù†§©Øµª≈ø∂≤≥∑ª∏∑π∂≤≤≤≥≥≥∏æπ∑∫º∑ØπÕ◊‹ÊÏÓıË›À∆«À »ªµ∂∫∏≤≠®®°õõû££••¢ù°¶ß®¶£•ß™¥∏∑µ¥∞≥πøÃ–”‹ﬁ›’À¡π∑∏ª¿øºª≥∏≈œ’’Ÿ’
	  to simulate the matrix for the terrain we take every 8 symbols as one row the next 8 symbols as another row and etc.
	  in the loop this is achieved by multiplying the first counter by number of columns and summing with the second counter
	  so first row will have 0*8+0=0 , 0*8+1 = 1,...., 0*8+7 = 7
	  second row 1*8+0 = 8.... 1*8+7 = 15 and etc;
	  so finally the heightmap std::vector will have something like this
	  [0] = 41.625000
	  [1] = 41.291688
	  [2] = 40.541668
	  [3] = 39.875000
	  and etc
	  where the ascii codes of the symbols are taken for height
*/
void Terrain::LoadHeightmap() {
	//we read ascii values of the chars in the heightmap, so we dont need negative values.
	//Therefore we use unsigned
	std::vector<unsigned char> temp(m_nRows*m_nCols);

	std::ifstream fin;
	fin.open(m_strHeightmapFileName.c_str(), std::ios_base::binary);

	if( !fin ) {
		MessageBox(NULL,"Cant open heightmap for reading",NULL,NULL);
	}

	//in the first parameter we need the adress of the beginning of the std::vector.
	//from there, read is starting to write one char from the file into new element in the std::vector
	fin.read((char*)&temp[0], temp.size());

	fin.close();

	m_vHeightmap.resize(m_nRows*m_nCols);
	for(int i = 0; i < m_nRows; ++i) {
		for(int j = 0; j < m_nCols; ++j) {
			m_vHeightmap[i*m_nCols+j] = static_cast<float>(temp[i*m_nCols+j]*m_fHeightsScale);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/*
Builds the mesh which represents our terrain.
	  Mesh is consisted of vertex and index buffer so we manualy write the information to our own vertex and index buffers
	  when invoking GenerateIndexBuffer() and GenerateVertexBuffer().After the mesh is created it is splited by 33x33 submeshes so large 
	  terrains can be loaded(512x512 for instance).The idea is that bounding boxes are generated for the submeshes and then it can be tested 
	  if the submesh is visible by the camera.If it is not visible it isnt rendered.
*/
void Terrain::GenerateTerrainMesh() {
	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;
	GenerateVertexBuffer(verts,m_nRows,m_nCols);
	GenerateIndexBuffer(indices,m_nRows,m_nCols);

	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;

	//VertexPositionNormalTexture vertexDeclaration;
	pApp->GetPNTDecl()->GetDeclaration(elems, &numElems);

	ID3DXMesh* mesh = 0;
	CheckSuccess(D3DXCreateMesh(m_nNumTriangles, m_nNumVertices, D3DPOOL_SCRATCH | D3DXMESH_32BIT, elems, pApp->GetDevice(), &mesh));

	VertexPositionNormalTexture* pVertexBuffer = 0;
	//LockVertexBuffer() gives us access to the internal mesh data and by v we can write information to the mesh
	mesh->LockVertexBuffer(0, (void**)&pVertexBuffer);

	for(UINT i = 0; i < mesh->GetNumVertices(); ++i) {
		pVertexBuffer[i].m_vPos   = verts[i];
		pVertexBuffer[i].m_vPos.y = m_vHeightmap[i];


		/*
		At this point the terrain is centered at 0,0,0 like this
					  +z
					   |
					 ----- row 0
					 | | | row 1
			 -x ------------------ +x
					 | | | row 2
					 ----- row n
					   |
					   |
					  -z
		We now want to save texture coordinates for the blendmap. Blendmap is texture that sits on top of the terrain and got the same dimension as it.
		Blendmap is used so multi-texturing can be performed. The texture coordinates are expressed in this coordinate system:
					
					 ----------- +u
					 | 
					 |
					 |
					 +v
		The texture coordinates must be in [0,1] range. So we have to translate the terrain to the fourth quadrant, so it starts from 0,0,0
		(this is done with + 0.5*width and -0.5*depth) and after that we have to divide by width and - depth, so the coordinates will be
		in [0,1] range. We divide by -depth, because in texture coordinates the +v goes down, while in the terrain the rows goes in negative z.
		*/
		pVertexBuffer[i].m_vTexCoords.x = (pVertexBuffer[i].m_vPos.x + (0.5f*m_nWidth)) / m_nWidth;
		pVertexBuffer[i].m_vTexCoords.y = (pVertexBuffer[i].m_vPos.z - (0.5f*m_nDepth)) / -m_nDepth;
	}
	
	//fill the index buffer in the mesh
	DWORD* index = 0;
	mesh->LockIndexBuffer(0, (void**)&index);
	for(UINT i = 0; i < mesh->GetNumFaces(); ++i) {
		//we multiply by 3 to jump to the next triangle
		index[i*3+0] = indices[i*3+0];
		index[i*3+1] = indices[i*3+1];
		index[i*3+2] = indices[i*3+2];
	}
	mesh->UnlockIndexBuffer();

	//compute normals
	D3DXComputeNormals(mesh, 0);
	
	//divide the whole mesh into parts
	int subGridRows = (m_nRows-1) / (k_nSubGridsRowsNumber-1);
	int subGridCols = (m_nCols-1) / (k_nSubGridsColsNumber-1);

	for(int r = 0; r < subGridRows; r++) {
		for(int c = 0; c < subGridCols; c++) {
			//generates the dimensions of the current sub-grid
			//the idea is to split the terrain in smaller parts so these parts can be tested for visibility,
			//and if some of the sub-grids are not visible, they are not rendered
			//if the whole terrain looks something like this:
		    //      ------ row 0
			//		|    | row 1
			//      |    | row 2
			//      ------ row n
			//this code will split it into something like this(this drawing will only split the terrain by 4 parts)
			//      -------------
			//		| SG1 | SG2 | 
			//      -------------
			//      | SG3 | SG4 | 
			//      -------------
			RECT rSubGridRectangle = {c*(k_nSubGridsColsNumber-1),r*(k_nSubGridsRowsNumber-1),(c+1)*(k_nSubGridsColsNumber-1),(r+1)*(k_nSubGridsRowsNumber-1)};

			BuildSubGridMesh(rSubGridRectangle, pVertexBuffer); 
		}
	}
	mesh->UnlockVertexBuffer();
	ReleaseX(mesh);
}

/////////////////////////////////////////////////////////////////////////
/*
generates the vertex buffer so the vertices are properly positioned and there is distance between the vertices - the m_fDX and m_fDZ
._._._. - one vertex
|     | - this is m_fDX
.     .
|     |
.     .
|     |
._._._.
 |
 m_fDZ
*/
void Terrain::GenerateVertexBuffer(std::vector<D3DXVECTOR3>& vVertices, int nNumRows,int nNumCols) {
	vVertices.resize(nNumRows*nNumCols);

	//the vertices in the terrain are initially generated in the fourth quadrant.
	//I.e. we start from the first row, generate all the vertices on this row and continue on the next row
	//              +z
	//               |
	//               |
	//       -x ------------ +x
	//				 |------ row 0
	//				 ||    | row 1
	//               ||    | row 2
	//               |------ row n
	//              -z
	//After that we translate the terrain around the coordinate system in 0,0,0 by the xOffset and zOffset
	//              +z
	//               |
	//             ----- row 0
	//			   | | | row 1
	//     -x ------------------ +x
	//			   | | | row 2
	//			   ----- row n
	//               |
	//               |
	//              -z
	//
	float xOffset = -m_nWidth * 0.5f; 
	float zOffset =  m_nDepth * 0.5f;

	//with m_fDX and m_fDZ we can scale the distance between each vertex in the terrain. for instance:
	//      .--.--.--.
	//		|		 |         Here m_fDX has lenght of 2, meaning that between two vertices on a row there is distance of 2 world space units
	//		.--.--.--.
	//
	//      .---.---.---.
	//		|			|      Here m_fDX has lenght of 3, meaning that between two vertices on a row there is distance of 3 world space units
	//		.---.---.---.
	//

	int k = 0;
	for(float i = 0; i < nNumRows; ++i) {
		for(float j = 0; j < nNumCols; ++j) {
			vVertices[k].x =  j * m_fDX + xOffset;
			//-i because we generate the vertices in the fourth quadrant(negative z goes down)
			vVertices[k].z = -i * m_fDZ + zOffset;
			//the height is later modified by the heightmap
			vVertices[k].y =  0.0f;
			
			k++; // Next vertex
		}
	}
}

/////////////////////////////////////////////////////////////////////////

//generates the indexBuffer for the mesh.
void Terrain::GenerateIndexBuffer(std::vector<DWORD>& vIndices,int nNumRows,int nNumCols) {
	vIndices.resize((nNumRows-1)*(nNumCols-1)*6);
	 
	/*
		Imagine this part  of a terrain( it is shown just two rows with 3 columns and 4 triangles).
		The idea of the indices is to associate every vertex with a number(his index). 
		If two or more vertices have same index(same number) only one vertex will be rendered, instead of all of them.
		This is extremely usefull for rendering large terrains where up to 6 vertices can be at the same position 
		and will be pointless to render all of them if they occupy the same position.

		j		j+1      j+2
	  i	-------------------
		|		/|		 /|
		|	   / |	    / |
		|  1  /	 |	   /  |
		|	 /	 |	  /   |
		|	/	 |   /	  |
		|  /  2	 |  /	  |
		| /		 | /	  |
		|/		 |/	      |
	i+1 -------------------
	
	for triangle 1 we have 3 vertices: (i;j)  , (i;j+1),(i+1;j)
	for triangle 2 we have 3 vertices: (i+1;j), (i;j+1),(i+1;j+1)
	
	Obviously here we have two vertices from triangle 1 and 2 that share the same positon( (i+1;j) and (i;j+1) )
	To generate their indices we will sum their column and row values(for example i+1 + j). 
	This way if we have vertices at the same position, they will have the same index value and only one of the vertices will be rendered.
	*/

	// Generate indices. We multiply by nNumCols in the calculations so we can jump on the next row. 
	// Remember that the heightmap is stored in linear array and the only way to go to next row is to multiply by the columns in this row.
	int k = 0;
	for (DWORD i = 0; i < (DWORD)(nNumRows-1); ++i) {
		for (DWORD j = 0; j < (DWORD)(nNumCols-1); ++j) {
			vIndices[k]	  = i* nNumCols+j;
			vIndices[k+1] = i* nNumCols+j+1;
			vIndices[k+2] = (i+1)*nNumCols+j;
					
			vIndices[k+3] = (i+1)*nNumCols+j;
			vIndices[k+4] = i*nNumCols+j+1;
			vIndices[k+5] = (i+1)*nNumCols+j+1;

			// next quad
			k += 6;
		}
	}
}

/////////////////////////////////////////////////////////////////////////

//checks if certain point is at the terrain
bool Terrain::IsInBounds(int i, int j) {
	return i >= 0 && i < m_nRows && j >= 0 && j < m_nCols;
}

/////////////////////////////////////////////////////////////////////////

void Terrain::OnLostDevice() {
	m_pEffect->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void Terrain::OnResetDevice() {
	m_pEffect->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

//Generates the mesh for the subgrid of the terrain
void Terrain::BuildSubGridMesh(RECT& rSubGridRectangle, VertexPositionNormalTexture* pVertices) {
	std::vector<D3DXVECTOR3> vTempVerts;
	std::vector<DWORD> vTempIndices;

	GenerateVertexBuffer(vTempVerts,k_nSubGridsRowsNumber,k_nSubGridsColsNumber);
	GenerateIndexBuffer(vTempIndices,k_nSubGridsRowsNumber,k_nSubGridsColsNumber);

	ID3DXMesh* pSubMesh = NULL;
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT nNumElements = 0;

	pApp->GetPNTDecl()->GetDeclaration(elems, &nNumElements);

	D3DXCreateMesh(k_nSubGridsTrianglesNumber, k_nSubGridsVertsNumber, D3DXMESH_MANAGED, elems, pApp->GetDevice(), &pSubMesh);

	VertexPositionNormalTexture* pVertexBuffer = 0;
	pSubMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);
	int k = 0;
	for(int i = rSubGridRectangle.top; i <= rSubGridRectangle.bottom; i++) {
		for(int j = rSubGridRectangle.left; j <= rSubGridRectangle.right; j++,k++) {
			pVertexBuffer[k] = pVertices[i*m_nCols+j];
		}
	}

	AABB bndBox;
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertexBuffer, pSubMesh->GetNumVertices(), 
		sizeof(VertexPositionNormalTexture), &bndBox.GetMinPoint(), &bndBox.GetMaxPoint());

	pSubMesh->UnlockVertexBuffer();

	WORD* pIndices  = 0;
	DWORD* pAttributeBuffer = 0;
	pSubMesh->LockIndexBuffer(0, (void**)&pIndices);
	pSubMesh->LockAttributeBuffer(0, &pAttributeBuffer);
	for(int i = 0; i < k_nSubGridsTrianglesNumber; ++i) {
		pIndices[i*3+0] = (WORD)vTempIndices[i*3+0];
		pIndices[i*3+1] = (WORD)vTempIndices[i*3+1];
		pIndices[i*3+2] = (WORD)vTempIndices[i*3+2];

		//indicate that all the triangles in the subGrid mesh will share the same attribute buffer
		//when we draw the subgrids we will call DrawSubset(0), which will render all the triangles in the subgrid with attribute ID = 0
		pAttributeBuffer[i] = 0; 
	}
	pSubMesh->UnlockIndexBuffer();
	pSubMesh->UnlockAttributeBuffer();

	TerrainSubGrid* subGrid = new TerrainSubGrid(pSubMesh,bndBox);

	m_vSubGrids.push_back(subGrid);
}


/////////////////////////////////////////////////////////////////////////

//here float x and float z are world coordinates, not elements in terrain mesh.You can place model at x = 15.0 and z = 20.0
//and then GetHeight will give you the height at this position
float Terrain::GetHeight(float x, float z) {
	//converts to fourth quadrant(with +0.5*width and -0.5*height), this is done
	//because this way it will be easier to detect later in which column and row our x and z are, since we first generated our terrain in fourth quadrant
	//also we divide by -m_fDZ to flip the sign of z axis, since currently it goes in negative way.

	//when we generated the terrain we had number of columns, number of rows and the distance between the vertices
	//across x axis(m_fDX) and the distance between the vertices across the z axis(m_fDZ).
	//then when we create the verticies we multiplied the column by m_fDX and the row m_fDZ, to get the vertices positions
	//Now after we have transformed to the fourth quadrant we have the vertices positions and to get the column and row
	//we have to divide by m_fDX and m_fDZ after we transform to four quadrant with (x + 0.5f*m_nWidth) and (z - 0.5f*m_nDepth)
	float fPossibleColumn = (x + 0.5f*m_nWidth)/m_fDX;
	float fPossibleRow    = (z - 0.5f*m_nDepth)/-m_fDZ;

	//floorf returns the largest integer less than or equal to the passed parameter
	int nRow = static_cast<int>(floorf(fPossibleRow));
	int nCol = static_cast<int>(floorf(fPossibleColumn));
	//we now have real column and row values in the terrain - nRow,nCol

	/*       nCol    nCol+1
		nRow     A.---.B
			     | w/ | 
			     | /  |
		nRow+1   C.---.D 
		
		w is the position, which height we want to find. Since only the points A,B,C,D have height values and w
		is probably inside ABC triangle or BCD triangle, we have to find out in which of the two triangle exactly lies in.
		After that based on the height values of the points in the triangle we will interpolate to find the height value of w
		*/

	//Our heights are stored in linear array, 
	//i.e. to simulate the format of the terrain(matrix) we have to multiply by m_nCols to jump on the next row
	float A = m_vHeightmap[nRow*m_nCols+nCol];
	float B = m_vHeightmap[nRow*m_nCols+(nCol+1)];
	float C = m_vHeightmap[(nRow+1)*m_nCols+nCol];
	float D = m_vHeightmap[(nRow+1)*m_nCols+(nCol+1)];

	//calculates the amount inside the triangle
	float s = fPossibleColumn - static_cast<float>(nCol);
	float t = fPossibleRow - static_cast<float>(nRow);

	//note that after we divide by m_fDX and m_fDZ, the distance bitween every column and row is 1
	//if t is less than 1-s we are in the upper triangle (ABC)
	if(t < 1.0f - s) {
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	//else we are in the lower triangle(DCB)
	else  {
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

/////////////////////////////////////////////////////////////////////////

bool Terrain::IsValidPosition(float x, float z) {
	float fPossibleColumn = (x + 0.5f*m_nWidth) / m_fDX;
	float fPossibleRow = (z - 0.5f*m_nDepth) / -m_fDZ;

	//floorf returns the largest integer less than or equal to the passed parameter
	int nRow = static_cast<int>(floorf(fPossibleRow));
	int nCol = static_cast<int>(floorf(fPossibleColumn));

	if( nRow > m_nRows || nCol > m_nCols || nRow < 0 || nCol < 0 ) {
		return false;
	}

	auto size = m_vHeightmap.size();
	if( nRow*m_nCols + nCol < size &&
	    nRow*m_nCols + (nCol + 1) < size &&
		(nRow + 1)*m_nCols + nCol  < size &&
		(nRow + 1)*m_nCols + (nCol + 1) < size) {
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
//used in shader
void Terrain::SetLightVector(D3DXVECTOR3 vLightVector) {
	m_pEffect->SetValue(m_hLightVector, &vLightVector, sizeof(D3DXVECTOR3));
}

/////////////////////////////////////////////////////////////////////////

//loads the effect parameters and the textures used by the terrain into the shader
void Terrain::BuildEffect() {
	CheckSuccess(D3DXCreateEffectFromFile(pApp->GetDevice(), "../../Resources/shaders/TerrainShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, 0));

	m_hEffectTechnique	= m_pEffect->GetTechniqueByName("TerrainTech");
	m_hWVPMatrix		= m_pEffect->GetParameterByName(0, "WVP");
	m_hBlendMap			= m_pEffect->GetParameterByName(0,"blendMap");
	m_hStoneTexture		= m_pEffect->GetParameterByName(0,"tex0");
	m_hDirtTexture		= m_pEffect->GetParameterByName(0,"tex1");
	m_hGrassTexture		= m_pEffect->GetParameterByName(0,"tex2");
	m_hLightVector		= m_pEffect->GetParameterByName(0, "lightVector");

	//sets the 3 textures and the blend map into the shader
	m_pEffect->SetTexture(m_hBlendMap,m_pBlendMapTexture);
	m_pEffect->SetTexture(m_hStoneTexture,m_pStoneTexture);
	m_pEffect->SetTexture(m_hDirtTexture,m_pDirtTexture);
	m_pEffect->SetTexture(m_hGrassTexture,m_pGrassTexture);
}


/////////////////////////////////////////////////////////////////////////

void Terrain::OnRender(const std::unique_ptr<Camera>& camera) {
	std::vector<TerrainSubGrid*> visibleSubGrids;
	for (auto& subGrid : m_vSubGrids) {
		if (camera->IsBoundingBoxVisible(subGrid->m_subGridBoungingBox)) {
			visibleSubGrids.push_back(subGrid);
		}
	}

	//dont compute anything for subgrids that are not visible
	D3DXMATRIX viewProjMatrix = camera->GetViewProjMatrix();
	m_pEffect->SetMatrix(m_hWVPMatrix, &viewProjMatrix);
	m_pEffect->SetTechnique(m_hEffectTechnique);
	UINT numPasses = 0;
	m_pEffect->Begin(&numPasses, 0);
	m_pEffect->BeginPass(0);

		for (auto& visibleSubGrid : visibleSubGrids) {
			visibleSubGrid->m_pSubGridMesh->DrawSubset(0);
		}
	
	m_pEffect->EndPass();
	m_pEffect->End();
}


/////////////////////////////////////////////////////////////////////////

