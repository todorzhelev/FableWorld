#include"StaticMesh.h"
#include<vector>
//clean renderbindedweapon and render


/////////////////////////////////////////////////////////////////////////
/*
Function:StaticMesh
Purpose:constructor
*/
StaticMesh::StaticMesh()
{
	//default texture for models that dont have any
	D3DXCreateTextureFromFile(pDxDevice, "textures/whitetex.dds", &m_pWhiteTexture);	

	m_light.m_vLight   = D3DXVECTOR3(20.0f, 300.0f, 50.0f);
	m_light.m_ambient  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_light.m_diffuse  = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_light.m_specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	D3DXVec3Normalize(&m_light.m_vLight, &m_light.m_vLight);

	BuildEffect();
}


////////////////////////////////////////////////////////////////////////
/*
Function:SkinnedMesh
Purpose:this constructor loads default values for the skinned mesh
*/
StaticMesh::StaticMesh(string strModelName, string ModelFileName, string strTextureFileName)
{
		//default texture for models that dont have any
	D3DXCreateTextureFromFile(pDxDevice, "textures/whitetex.dds", &m_pWhiteTexture);	

	m_light.m_vLight   = D3DXVECTOR3(20.0f, 300.0f, 50.0f);
	m_light.m_ambient  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_light.m_diffuse  = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_light.m_specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	D3DXVec3Normalize(&m_light.m_vLight, &m_light.m_vLight);

	BuildEffect();

	m_vLook		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vRight	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUp		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_vTitleLook	= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vTitleRight	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vTitleUp	 	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_vTitleForQuestLook		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vTitleForQuestRight		= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vTitleForQuestUp			= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_fTitleForQuestRotationAngleByY	= 0.0;

	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_fScale = 0.3f;

	m_fRotAngleX = 0;

	m_fRotAngleY = 0;

	m_fTitleRotationAngleByY = 0;

	m_strModelName = strModelName;

	m_strTitleForQuest = "?";

	m_strModelFileName = ModelFileName;

	m_strTextureFileName = strTextureFileName;

	m_strActorType = "neutral";

	m_bIsBindable = false;

	D3DXCreateTextureFromFile(pDxDevice, strTextureFileName.c_str(), &m_pTexture);	

	/*if( pMesh->m_bIsBindable && !pMesh->m_strBindedToAnimatedModelName.empty() && !pMesh->m_strBindedToBoneName.empty() )
	{
		SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>(pEngine->m_mapGameObjects.find(pMesh->m_strBindedToAnimatedModelName)->second);

		pSkinnedMesh->BindWeaponToModel(pMesh->m_strModelName,pMesh->m_strBindedToBoneName);
	}*/

	m_eGameObjectType = EGameObjectType_Static;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:LoadXFile
Purpose:load model from .x file
*/
void StaticMesh::LoadGameObject()
{
	ID3DXMesh* pMesh			 = NULL;
	ID3DXBuffer* pMaterialBuffer = NULL;

	DWORD nMaterialsAmount = 0;

	D3DXLoadMeshFromX(m_strModelFileName.c_str(), D3DXMESH_SYSTEMMEM, pDxDevice, 0,	&pMaterialBuffer, 0, &nMaterialsAmount, &pMesh);

	bool bHasNormals = HasNormals(pMesh);

	D3DVERTEXELEMENT9 elements[64];
	UINT nElementsAmount = 0;

	VertexPositionNormalTexture vertexDeclaration;
	pApp->GetPositionNormalTextureDecl()->GetDeclaration(elements, &nElementsAmount);

	ID3DXMesh* pTempMesh = 0;
	pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, elements, pDxDevice, &pTempMesh);
	pMesh->Release();
	pMesh = pTempMesh;


	//generate normals if the models doesnt have
	if( !bHasNormals )
	{
		D3DXComputeNormals(pMesh, 0);
	}

	pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, elements, pDxDevice, &(m_pMesh));
	pMesh->Release();

	//one mesh can be divided by different parts, called subsets and for every substet
	//there is different material and different texture associated with it
	//for example car can be divided by 3 parts - the car, front rims and back rims. 
	//These 3 subsets will have different material and different texture associated with them
	//this code reads from the information, stored in the .x file and saves it in two vectors: mMtrl and mTex
	if( pMaterialBuffer != 0 && nMaterialsAmount != 0 )
	{
		D3DXMATERIAL* d3dxmtrls = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

		for(DWORD i = 0; i < nMaterialsAmount; ++i)
		{
			Material material;
			material.m_ambient		  = d3dxmtrls[i].MatD3D.Diffuse;
			material.m_diffuse		  = d3dxmtrls[i].MatD3D.Diffuse;
			material.m_specular		  = d3dxmtrls[i].MatD3D.Specular;
			material.m_fSpecularPower = d3dxmtrls[i].MatD3D.Power;

			m_vMaterials.push_back(material);

			if( d3dxmtrls[i].pTextureFilename != 0 )
			{
				IDirect3DTexture9* pTexture = 0;
				char strTexturePath[80];
				strcpy_s(strTexturePath,"textures/");
				strcat_s(strTexturePath,d3dxmtrls[i].pTextureFilename);
				D3DXCreateTextureFromFile(pDxDevice, strTexturePath, &pTexture);

				m_vTextures.push_back(pTexture);
			}
			else
			{
				m_vTextures.push_back(NULL);
			}
		}
	}
	pMaterialBuffer->Release();

	//create bounding box
	BuildBoundingBox();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:BuildBoundingBox
Purpose:generates bounding box for a model
*/
void StaticMesh::BuildBoundingBox()
{
	VertexPositionNormalTexture* pVertexBuffer = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);

	D3DXComputeBoundingBox(&pVertexBuffer[0].m_vPos, m_pMesh->GetNumVertices(), sizeof(VertexPositionNormalTexture), &(m_BoundingBox.minPt), &(m_BoundingBox.maxPt));
	
	m_pMesh->UnlockVertexBuffer();
	
    float width  = m_BoundingBox.maxPt.x - m_BoundingBox.minPt.x;
	float height = m_BoundingBox.maxPt.y - m_BoundingBox.minPt.y;
	float depth  = m_BoundingBox.maxPt.z - m_BoundingBox.minPt.z;

	fout<<"Bounding box\n"<<width<<endl<<height<<endl<<depth<<endl;

	D3DXCreateBox(pDxDevice, width, height, depth, &m_pBoundingBoxMesh, 0);

	D3DXVECTOR3 center = m_BoundingBox.center();
	D3DXMatrixTranslation(&(m_BoundingBoxOffset), center.x, center.y, center.z);

	m_BoundingBoxMaterial.m_ambient  		= D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_diffuse   		= D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_specular     	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.17f);
	m_BoundingBoxMaterial.m_fSpecularPower 	= 8.0f;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invoked when the device is reset
*/
void StaticMesh::OnResetDevice()
{
	m_pEffect->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invoked when the device is lost
*/
void StaticMesh::OnLostDevice()
{
	//add check if it already went through here
	m_pEffect->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:update
Purpose:updates the position of the mesh
*/
void StaticMesh::OnUpdate(float fDeltaTime)
{
	//binded models got their own height, based on the bone that they are attached to, other models are just put on the terrain
	/*if(!m_bIsBindable)
	{
		m_vPos.y = pTerrain->GetHeight(m_vPos.x,m_vPos.z);
	}*/
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsPicked
Purpose:checks if the mouse is over the bounding box
*/
bool StaticMesh::IsPicked(D3DXMATRIX TransformationMatrix,D3DXVECTOR3 origin,D3DXVECTOR3 dir)
{
	AABB box;
	bool bIsPicked = false;
	
	m_BoundingBox.transformByMatrix(TransformationMatrix, box);
	bIsPicked = D3DXBoxBoundProbe(&box.minPt, &box.maxPt, &origin, &dir);
	
	return bIsPicked;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:buildEffect
Purpose:set shader parameters
*/
void StaticMesh::BuildEffect()
{
	if( FAILED(D3DXCreateEffectFromFile(pDxDevice,"shaders/StaticMeshShader.fx",0,0,D3DXSHADER_DEBUG,0,&m_pEffect,0)) )
	{
		MessageBox(0,"Failed loading effect file in static mesh",0,0);
		PostQuitMessage(0);
	}

	m_hEffectTechnique	= m_pEffect->GetTechniqueByName("StaticMeshTech");
	m_hWVPMatrix		= m_pEffect->GetParameterByName(0, "WVP");
	m_hMaterial			= m_pEffect->GetParameterByName(0, "mtrl");
	m_hLight			= m_pEffect->GetParameterByName(0, "light");
	m_hTexture			= m_pEffect->GetParameterByName(0, "text");

	m_pEffect->SetValue(m_hLight, &m_light, sizeof(Light));
}


/////////////////////////////////////////////////////////////////////////
/*
Function:render
Purpose:renders not-binded model
*/
void StaticMesh::OnRender()
{
	//if the model is not binded to skinned mesh's bone render it
	if(!m_bIsBindable)
	{
		//vectors for picking
		D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

		m_pEffect->SetTechnique(m_hEffectTechnique);

		D3DXMATRIX T,T1;
		D3DXMATRIX S;
		D3DXMATRIX R;
		D3DXMATRIX R1;

		D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
		D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
		D3DXMatrixRotationY(&R1,m_fRotAngleY);
		D3DXMATRIX CombinedMatrix = S*R1*T;

		//alpha channel is used in the trees
		pDxDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		pDxDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pDxDevice->SetRenderState(D3DRS_ALPHAREF, 100);

		m_pEffect->SetMatrix(m_hWVPMatrix,&(CombinedMatrix *(camera->GetViewProjMatrix())));	

		UINT numPasses = 0;
		m_pEffect->Begin(&numPasses,0);
		for(UINT i =0;i<numPasses;++i)
		{
			m_pEffect->BeginPass(i);
				
				for(unsigned int j = 0; j < m_vMaterials.size(); ++j)
				{
					m_pEffect->SetValue(m_hMaterial, &(m_vMaterials[j]), sizeof(Material));
					
					if(m_vTextures[j] != NULL)
					{
						m_pEffect->SetTexture(m_hTexture, m_vTextures[j]);
					}
					else
					{
						m_pEffect->SetTexture(m_hTexture, m_pWhiteTexture);
					}
										
					m_pEffect->CommitChanges();

					m_pMesh->DrawSubset(j);					
				}

				//check for picking
				if( pDinput->IsMouseButtonDown(0) )
				{
					GetWorldPickingRay(vOrigin, vDir);
				
					if( IsPicked(CombinedMatrix,vOrigin,vDir) )
					{
						////by finding the inverse of mesh CombinedMatrix we transform from World space to the mesh local space
						////this is needed, because we have to transform the picking vectors to the mesh local space
						////which is required by the D3DXIntersect function
						//D3DXMATRIX InverseWorldMatrix;
						//D3DXMatrixInverse(&InverseWorldMatrix, 0, &CombinedMatrix);

						////transform the Ray using the inverse matrix
						//D3DXVec3TransformCoord(&vOrigin, &vOrigin, &InverseWorldMatrix);
						//D3DXVec3TransformNormal(&vDir, &vDir, &InverseWorldMatrix);

						//BOOL hit = false;
						//DWORD faceIndex = -1;
						//float u = 0.0f;
						//float v = 0.0f;
						//float dist = 0.0f;
						//ID3DXBuffer* allhits = 0;
						//DWORD numHits = 0;
						//D3DXIntersect(m_pMesh, &vOrigin, &vDir, &hit,&faceIndex, &u, &v, &dist, &allhits, &numHits);
						//release(allhits);

						//if( hit )
						{
							m_bIsPicked = true;
						}		
					}
				}

				RenderBoundingBox();

			m_pEffect->EndPass();
		}
		m_pEffect->End();
	}
	pDxDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
}

/////////////////////////////////////////////////////////////////////////
/*
Function:RenderBoundingBox
Purpose:renders the bounding box
*/
void StaticMesh::RenderBoundingBox()
{
	pDxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDxDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDxDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		D3DXMATRIX T,T1;
		D3DXMATRIX S;
		D3DXMATRIX R;
		D3DXMATRIX R1;

		D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
		D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
		D3DXMatrixRotationY(&R1,m_fRotAngleY);

	m_pEffect->SetMatrix(m_hWVPMatrix, &(m_BoundingBoxOffset*S*R1*T*camera->GetViewProjMatrix()));
	m_pEffect->SetValue(m_hMaterial, &m_BoundingBoxMaterial, sizeof(Material));
	m_pEffect->SetTexture(m_hTexture, m_pWhiteTexture);
	m_pEffect->CommitChanges();

	m_pBoundingBoxMesh->DrawSubset(0);

	pDxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}


/////////////////////////////////////////////////////////////////////////