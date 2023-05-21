#include <stdafx.h>
#include "Water.h"
#include "Camera.h"

/////////////////////////////////////////////////////////////////////////

Water::Water(InitInfo& initInfo) {
	m_initInfo = initInfo;

	m_width = (initInfo.vertCols - 1) * initInfo.dx;
	m_depth = (initInfo.vertRows - 1) * initInfo.dz;

	m_waveNMapOffset0 = D3DXVECTOR2(0.0f, 0.0f);
	m_waveNMapOffset1 = D3DXVECTOR2(0.0f, 0.0f);

	m_waveDMapOffset0 = D3DXVECTOR2(0.0f, 0.0f);
	m_waveDMapOffset1 = D3DXVECTOR2(0.0f, 0.0f);

	DWORD numTris = (initInfo.vertRows - 1) * (initInfo.vertCols - 1) * 2;
	DWORD numVerts = initInfo.vertRows * initInfo.vertCols;

	D3DVERTEXELEMENT9 elements[MAX_FVF_DECL_SIZE];
	UINT numElements = 0;
	pApp->GetPositionNormalDisplacementDecl()->GetDeclaration(elements, &numElements);
	ID3DXMesh* mesh = nullptr;
	D3DXCreateMesh(numTris, numVerts, D3DXMESH_MANAGED | D3DXMESH_32BIT, elements, pApp->GetDevice(), &mesh);
	m_pMesh.reset(mesh);

	VertexPositionNormalDisplacement* v = nullptr;
	m_pMesh->LockVertexBuffer(0, (void**)&v);

	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;
	GenTriGrid(m_initInfo.vertRows, m_initInfo.vertCols, m_initInfo.dx, m_initInfo.dz, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	for (int i = 0; i < m_initInfo.vertRows; ++i) {
		for (int j = 0; j < m_initInfo.vertCols; ++j) {
			DWORD index = i * m_initInfo.vertCols + j;
			const auto coord = D3DXVECTOR2((float)j / m_initInfo.vertCols, (float)i / m_initInfo.vertRows);

			v[index].m_pos					= verts[index];
			v[index].m_normalMapCoord		= coord * initInfo.texScale;
			v[index].m_displacementMapCoord = coord;
		}
	}
	m_pMesh->UnlockVertexBuffer();

	DWORD* indexBuffPtr = 0;
	m_pMesh->LockIndexBuffer(0, (void**)&indexBuffPtr);
	DWORD* attBuff = 0;
	m_pMesh->LockAttributeBuffer(0, &attBuff);
	for (UINT i = 0; i < m_pMesh->GetNumFaces(); ++i) {
		indexBuffPtr[i*3+0] = indices[i*3+0];
		indexBuffPtr[i*3+1] = indices[i*3+1];
		indexBuffPtr[i*3+2] = indices[i*3+2];

		attBuff[i] = 0; // All in subset 0.
	}
	m_pMesh->UnlockIndexBuffer();
	m_pMesh->UnlockAttributeBuffer();

	DWORD* adj = new DWORD[m_pMesh->GetNumFaces() * 3];
	m_pMesh->GenerateAdjacency(EPSILON, adj);
	m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT, adj, 0, 0, 0);
	delete[] adj;

	IDirect3DTexture9* waveMap0 = nullptr;
	IDirect3DTexture9* waveMap1 = nullptr;
	D3DXCreateTextureFromFile(pApp->GetDevice(), initInfo.waveMapFilename0.c_str(), &waveMap0);
	D3DXCreateTextureFromFile(pApp->GetDevice(), initInfo.waveMapFilename1.c_str(), &waveMap1);
	m_pWaveMap0.reset(waveMap0);
	m_pWaveMap1.reset(waveMap1);

	IDirect3DTexture9* dispMap0 = nullptr;
	IDirect3DTexture9* dispMap1 = nullptr;
	D3DXCreateTextureFromFileEx(pApp->GetDevice(), initInfo.dmapFilename0.c_str(), m_initInfo.vertRows, m_initInfo.vertCols, 1, 0, D3DFMT_R32F, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &dispMap0);
	D3DXCreateTextureFromFileEx(pApp->GetDevice(), initInfo.dmapFilename1.c_str(), m_initInfo.vertRows, m_initInfo.vertCols, 1, 0, D3DFMT_R32F, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &dispMap1);
	m_pDispMap0.reset(dispMap0);
	m_pDispMap1.reset(dispMap1);
	buildFX();
}

/////////////////////////////////////////////////////////////////////////

void Water::GenTriGrid(int numVertRows, int numVertCols, float dx, float dz, const D3DXVECTOR3& center, std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices) {
	int numVertices = numVertRows * numVertCols;
	int numCellRows = numVertRows - 1;
	int numCellCols = numVertCols - 1;

	int numTris = numCellRows * numCellCols * 2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	verts.resize(numVertices);

	// Offsets to translate grid from quadrant 4 to center of coordinate system.
	float xOffset = -width * 0.5f;
	float zOffset = depth * 0.5f;

	int k = 0;
	for (float i = 0; i < numVertRows; ++i) {
		for (float j = 0; j < numVertCols; ++j) {
			// Negate the depth coordinate to put in quadrant four. Then offset to center about coordinate system.
			verts[k].x = j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y = 0.0f;

			// Translate so that the center of the grid is at the specified 'center' parameter.
			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);

			++k; // Next vertex
		}
	}

	indices.resize(numTris * 3);

	k = 0;
	for (DWORD i = 0; i < (DWORD)numCellRows; ++i) {
		for (DWORD j = 0; j < (DWORD)numCellCols; ++j) {
			indices[k] = i * numVertCols + j;
			indices[k + 1] = indices[k + 4] = i * numVertCols + j + 1;
			indices[k + 2] = indices[k + 3] = (i + 1) * numVertCols + j;
			indices[k + 5] = (i + 1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Water::buildFX() {
	ID3DXBuffer* errors = nullptr;
	ID3DXEffect* effect = nullptr;
	D3DXCreateEffectFromFile(pApp->GetDevice(), "../../Resources/shaders/WaterShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &effect, &errors);
	if (errors) {
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	}

	m_pFX.reset(effect);

	m_hTech			   = m_pFX->GetTechniqueByName("WaterTech");
	m_hWorld		   = m_pFX->GetParameterByName(0, "gWorld");
	m_hWorldInv		   = m_pFX->GetParameterByName(0, "gWorldInv");
	m_hWVP			   = m_pFX->GetParameterByName(0, "gWVP");
	m_hEyePosW		   = m_pFX->GetParameterByName(0, "gEyePosW");
	m_hLight		   = m_pFX->GetParameterByName(0, "gLight");
	m_hMtrl			   = m_pFX->GetParameterByName(0, "gMtrl");
	m_hWaveMap0		   = m_pFX->GetParameterByName(0, "gWaveMap0");
	m_hWaveMap1		   = m_pFX->GetParameterByName(0, "gWaveMap1");
	m_hWaveNMapOffset0 = m_pFX->GetParameterByName(0, "gWaveNMapOffset0");
	m_hWaveNMapOffset1 = m_pFX->GetParameterByName(0, "gWaveNMapOffset1");
	m_hWaveDMapOffset0 = m_pFX->GetParameterByName(0, "gWaveDMapOffset0");
	m_hWaveDMapOffset1 = m_pFX->GetParameterByName(0, "gWaveDMapOffset1");
	m_hWaveDispMap0	   = m_pFX->GetParameterByName(0, "gWaveDispMap0");
	m_hWaveDispMap1    = m_pFX->GetParameterByName(0, "gWaveDispMap1");
	m_hScaleHeights    = m_pFX->GetParameterByName(0, "gScaleHeights");
	m_hGridStepSizeL   = m_pFX->GetParameterByName(0, "gGridStepSizeL");
	//m_hEnvMap		  = m_pFX->GetParameterByName(0, "gEnvMap");


	m_pFX->SetMatrix(m_hWorld, &m_initInfo.toWorld);
	D3DXMATRIX worldInv;
	D3DXMatrixInverse(&worldInv, 0, &m_initInfo.toWorld);
	m_pFX->SetMatrix(m_hWorldInv, &worldInv);
	m_pFX->SetTechnique(m_hTech);
	m_pFX->SetTexture(m_hWaveMap0, m_pWaveMap0.get());
	m_pFX->SetTexture(m_hWaveMap1, m_pWaveMap1.get());
	m_pFX->SetTexture(m_hWaveDispMap0, m_pDispMap0.get());
	m_pFX->SetTexture(m_hWaveDispMap1, m_pDispMap1.get());
	m_pFX->SetValue(m_hLight, &m_initInfo.dirLight, sizeof(Light));
	m_pFX->SetValue(m_hMtrl, &m_initInfo.mtrl, sizeof(Material));
	m_pFX->SetValue(m_hScaleHeights, &m_initInfo.scaleHeights, sizeof(D3DXVECTOR2));

	D3DXVECTOR2 stepSizes(m_initInfo.dx, m_initInfo.dz);
	m_pFX->SetValue(m_hGridStepSizeL, &stepSizes, sizeof(D3DXVECTOR2));
}

/////////////////////////////////////////////////////////////////////////

void Water::OnUpdate(float dt) {
	// Update texture coordinate offsets.  These offsets are added to the texture coordinates in the vertex shader to animate them.
	m_waveNMapOffset0 += m_initInfo.waveNMapVelocity0 * dt;
	m_waveNMapOffset1 += m_initInfo.waveNMapVelocity1 * dt;

	m_waveDMapOffset0 += m_initInfo.waveDMapVelocity0 * dt;
	m_waveDMapOffset1 += m_initInfo.waveDMapVelocity1 * dt;

	// Textures repeat every 1.0 unit, so reset back down to zero so the coordinates do not grow too large.
	if (m_waveNMapOffset0.x >= 1.0f || m_waveNMapOffset0.x <= -1.0f)
		m_waveNMapOffset0.x = 0.0f;
	if (m_waveNMapOffset0.y >= 1.0f || m_waveNMapOffset0.y <= -1.0f)
		m_waveNMapOffset0.y = 0.0f;
	if (m_waveNMapOffset1.x >= 1.0f || m_waveNMapOffset1.x <= -1.0f)
		m_waveNMapOffset1.x = 0.0f;
	if (m_waveNMapOffset1.y >= 1.0f || m_waveNMapOffset1.y <= -1.0f)
		m_waveNMapOffset1.y = 0.0f;

	if (m_waveDMapOffset0.x >= 1.0f || m_waveDMapOffset0.x <= -1.0f)
		m_waveDMapOffset0.x = 0.0f;
	if (m_waveDMapOffset0.y >= 1.0f || m_waveDMapOffset0.y <= -1.0f)
		m_waveDMapOffset0.y = 0.0f;
	if (m_waveDMapOffset1.x >= 1.0f || m_waveDMapOffset1.x <= -1.0f)
		m_waveDMapOffset1.x = 0.0f;
	if (m_waveDMapOffset1.y >= 1.0f || m_waveDMapOffset1.y <= -1.0f)
		m_waveDMapOffset1.y = 0.0f;
}

/////////////////////////////////////////////////////////////////////////

void Water::OnRender(const std::unique_ptr<Camera>& camera) {
	const auto matrix = m_initInfo.toWorld * camera->GetViewProjMatrix();
	m_pFX->SetMatrix(m_hWVP, &matrix);
	m_pFX->SetValue(m_hEyePosW, &camera->GetPosition(), sizeof(D3DXVECTOR3));
	m_pFX->SetValue(m_hWaveNMapOffset0, &m_waveNMapOffset0, sizeof(D3DXVECTOR2));
	m_pFX->SetValue(m_hWaveNMapOffset1, &m_waveNMapOffset1, sizeof(D3DXVECTOR2));
	m_pFX->SetValue(m_hWaveDMapOffset0, &m_waveDMapOffset0, sizeof(D3DXVECTOR2));
	m_pFX->SetValue(m_hWaveDMapOffset1, &m_waveDMapOffset1, sizeof(D3DXVECTOR2));

	UINT numPasses = 0;
	m_pFX->Begin(&numPasses, 0);
	m_pFX->BeginPass(0);

		m_pMesh->DrawSubset(0);

	m_pFX->EndPass();
	m_pFX->End();
}

/////////////////////////////////////////////////////////////////////////

void Water::SetEnvMap(IDirect3DCubeTexture9* envMap) {
	m_pFX->SetTexture(m_hEnvMap, envMap);
}

/////////////////////////////////////////////////////////////////////////

void Water::OnLostDevice() {
	m_pFX->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void Water::OnResetDevice() {
	m_pFX->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

Water::~Water() {
}

/////////////////////////////////////////////////////////////////////////