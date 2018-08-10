#include <stdafx.h>
#include "SkinnedModel.h"
#include "StaticModel.h"
#include "AnimationComponent.h"

/////////////////////////////////////////////////////////////////////////

SkinnedModel::SkinnedModel()
{
	//default white texture for models which doesnt have any
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice, "../../Resources/textures/DefaultWhiteTexture.dds", &m_pWhiteTexture));

	//max number of bones that can be supported.Above 60 bones arent rendered correctly
	m_nMaxBonesSupported = 60;

	//light values
	m_light.m_vLight 	= D3DXVECTOR3(20.0f, 300.0f, 50.0f);
	D3DXVec3Normalize(&m_light.m_vLight, &m_light.m_vLight);
	m_light.m_ambient 	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_light.m_diffuse 	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_light.m_specular 	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	m_whiteMaterial.m_ambient	 		= WHITE*0.9f;
	m_whiteMaterial.m_diffuse 			= WHITE*0.6f;
	m_whiteMaterial.m_specular 			= WHITE*0.6f;
	m_whiteMaterial.m_fSpecularPower 	= 48.0f;

	m_bShouldRenderTitles = true;
	m_pAnimationComponent = new AnimationComponent();

	m_movementSpeed = 1;
	BuildEffect();
	BuildEffectForTitles();
}

/////////////////////////////////////////////////////////////////////////

SkinnedModel::SkinnedModel(std::string strModelName, std::string ModelFileName, std::string strTextureFileName,bool bShouldRenderTitles)
{
	//code duplication, move it to another function
	//default white texture for models which doesnt have any
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice, "../../Resources/textures/DefaultWhiteTexture.dds", &m_pWhiteTexture));

	m_pAnimationComponent = new AnimationComponent();

	//max number of bones that can be supported.Above 60 bones arent rendered correctly
	m_nMaxBonesSupported = 60;

	//light values
	m_light.m_vLight 	= D3DXVECTOR3(-2,31,-160);
	D3DXVec3Normalize(&m_light.m_vLight, &m_light.m_vLight);
	m_light.m_ambient 	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_light.m_diffuse 	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_light.m_specular 	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	m_whiteMaterial.m_ambient	 		= WHITE*0.9f;
	m_whiteMaterial.m_diffuse 			= WHITE*0.6f;
	m_whiteMaterial.m_specular 			= WHITE*0.6f;
	m_whiteMaterial.m_fSpecularPower 	= 48.0f;

	BuildEffect();
	BuildEffectForTitles();

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
	m_fRotAngleZ = 0;
	m_fTitleRotationAngleByY = 0;
	m_strModelName = strModelName;
	m_strTitleForQuest = "?";

	//cho_5anmx ezreal_5anm
	m_strModelFileName = ModelFileName;

	//chogath_abyss_TX_CM.DDS  Ezreal_ProdigalExplorer
	m_strTextureFileName = strTextureFileName;

	m_strActorType = "neutral";

	m_bIsAttacked = false;
	m_bIsAttacking = false;
	m_bIsDead = false;
	m_bIsPicked = false;
	m_bHasDialogue = false;
	m_strAttackerName = "";
	m_movementSpeed = 1;
	m_eGameObjectType = EGameObjectType_Skinned;

	m_bShouldRenderTitles = bShouldRenderTitles;
}
/////////////////////////////////////////////////////////////////////////

void SkinnedModel::LoadGameObject()
{
	//creates the texture for the model
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice, m_strTextureFileName.c_str(), &m_pTexture));

	AllocateHierarchy allocMeshHierarchy;

	CheckSuccess(D3DXLoadMeshHierarchyFromX(m_strModelFileName.c_str(), D3DXMESH_SYSTEMMEM, pDxDevice, &allocMeshHierarchy, 0, &m_pRoot, &m_pAnimationComponent->m_pAnimationController));

	m_pAnimationComponent->EnableTrack(FirstTrack, true);
	m_pAnimationComponent->EnableTrack(SecondTrack, true);

	//there should be only one bone containing the whole skinned mesh
	D3DXFRAME* pFrame = FindFrameWithMesh(m_pRoot);

	if( !pFrame ) 
	{
		fout<<"cannot find node with mesh"<<std::endl;
	}
	
	D3DXMESHCONTAINER* pMeshContainer = pFrame->pMeshContainer;
	m_pSkinInfo = pMeshContainer->pSkinInfo;

	m_nNumBones = pMeshContainer->pSkinInfo->GetNumBones();
	m_vFinalBonesMatrices.resize(m_nNumBones);
	m_vToRootMatrices.resize(m_nNumBones, 0);

	char str[80];
	strcpy_s(str,"../../Resources/textures/SkinnedModels/");
	strcat_s(str,pMeshContainer->pMaterials->pTextureFilename);

	m_strTextureFileName = str;

	//creates the texture for the model
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice, m_strTextureFileName.c_str(), &m_pTexture));

	//set the idle animation on the current track and mantain one more track, 
	//which we will use later to switch to attack or dead animations

	m_pAnimationComponent->SetAnimationOnTrack("idle",FirstTrack);

	m_pAnimationComponent->SetTrackSpeed(FirstTrack, 1.0f, 0.0f);
	m_pAnimationComponent->SetTrackWeight(FirstTrack, 1.0f, 0.0f);
 
	m_pAnimationComponent->SetTrackSpeed(SecondTrack, 0.0f, 0.0f);
	m_pAnimationComponent->SetTrackWeight(SecondTrack, 0.0f, 0.0f);
	
	//after the mesh is found it is then changed in order to become skinned mesh
	BuildSkinnedModel(pMeshContainer->MeshData.pMesh);

	InitBonesToRootMatricesPointersArray();
}

/////////////////////////////////////////////////////////////////////////

SkinnedModel::~SkinnedModel()
{
}

/////////////////////////////////////////////////////////////////////////

//returns the final matrices for each bone which are passed to the shader for rendering.
//We are returning the adress of the first element of the array, because in the shader we need only the beginning address of the array
//since the elements are in consecutive order
D3DXMATRIX* SkinnedModel::GetFinalBonesMatricesArray()
{
	return &m_vFinalBonesMatrices.front();
}

/////////////////////////////////////////////////////////////////////////

//finds the frame that contains mesh
D3DXFRAME* SkinnedModel::FindFrameWithMesh(D3DXFRAME* frame)
{
	if( frame->pMeshContainer )
	{
		if( frame->pMeshContainer->MeshData.pMesh != nullptr)
		{
			return frame;
		}
	}

	D3DXFRAME* pFrame = nullptr;
	if(frame->pFrameSibling)
	{
		pFrame = FindFrameWithMesh(frame->pFrameSibling);
		if( pFrame != nullptr )
		{
			return pFrame;
		}
	}

	if(frame->pFrameFirstChild)
	{
		pFrame = FindFrameWithMesh(frame->pFrameFirstChild);

		if( pFrame != nullptr )
		{
			return pFrame;
		}
	}

	return nullptr;
}

/////////////////////////////////////////////////////////////////////////

//creates the mesh and converts it to skinned mesh and also computes normals for the mesh if needed
void SkinnedModel::BuildSkinnedModel(ID3DXMesh* pMesh)
{
	D3DVERTEXELEMENT9 elements[64];
	UINT numElements = 0;

	pApp->GetPositionNormalTextureDecl()->GetDeclaration(elements, &numElements);

	ID3DXMesh* pTempMesh = 0;
	pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, elements, pDxDevice, &pTempMesh);
	 
	if( !HasNormals(pTempMesh) )
	{
		D3DXComputeNormals(pTempMesh, 0);
	}
	
	DWORD nNumBoneCombinations = 0;
	ID3DXBuffer* pBoneCombinationsTable = 0;

	m_pSkinInfo->ConvertToIndexedBlendedMesh( pTempMesh, 
										      D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,  
											  m_nMaxBonesSupported, 
										      0, 
										      0, 
											  0, 
											  0, 
											  &m_nMaxVertInfluences,
											  &nNumBoneCombinations, 
											  &pBoneCombinationsTable, 
											  &m_pMesh);
	pTempMesh->Release();
	pBoneCombinationsTable->Release();

	BuildBoundingBox();
}

/////////////////////////////////////////////////////////////////////////
/*
Stores pointers to the to root matrices of all the bones in the skinned mesh
The to root matrices will be updated every frame in OnUpdate, and we will have
quick access to these matrices just by index, instead of using D3DXFrameFind every time in OnUpdate
These matrices are needed to calculate the final matrix transformation for the vertices
*/
void SkinnedModel::InitBonesToRootMatricesPointersArray()
{
	for(UINT i = 0; i < m_nNumBones; ++i)
	{
		const char* boneName = m_pSkinInfo->GetBoneName(i);
		D3DXFRAME* pFrame	 = D3DXFrameFind(m_pRoot, boneName);
		
		if( pFrame )
		{
			Bone* pBone = static_cast<Bone*>( pFrame );
			m_vToRootMatrices[i] = &pBone->m_toRootMatrix;
		}
	}
}

/////////////////////////////////////////////////////////////////////////

//binds static object to animated model's bone
void SkinnedModel::BindWeaponToModel(std::string strObjectName,std::string strBoneToBind)
{
	GameObject* pGameObject = m_pGameObjManager->GetObjectByName(strObjectName.c_str());

	m_mapBindedObjects[pGameObject] = strBoneToBind;
}

/////////////////////////////////////////////////////////////////////////
/*
Traverse the whole ierarchy of frames and builds the to root matrices for every frame
here siblings are 2 or more bones that share same parent and child is bone with only one parent.
The code traverse through the siblings, if there are any, and combines the current bone transformation matrices(scaling,rotation,translation)
with the transformation matrices of the bone above him(his parent). This way if the parent bone moves the child bone has to move too.
After that if there are no more siblings, it finds the first child of this parent bone.
Note that if one bone got 2 or more children bones, it just takes the first child and from then on takes the other children as siblings to the first child

                . root
		       / \  
	 bone1    .   .  bone2
			 / \   \
	bone3	.   .   . bone 5
	          bone4

In this example we start from root and take its TransformationMatrix. We save this matrix in toRoot and check for siblings.
Here bone1 and bone2 are siblings of root and we invoke buildToRoot with the first sibling and the same matrix(parentsToRoot) that affects the root.
After bone1 matrices are calculated, we invoke buildToRoot with the next sibling(bone2) and again with the same matrix that affects the root.
After we calculated bone2 matrices we find that there are no more siblings so we find the first child of it(bone5)
Here thought we invoke buildToRoot with combined matrix from the matrix that affects the root and the transformation matrix of bone5's parent bone - bone2.
We do this, because we want when the root is moved all bones in the hierarchy to be moved too and also if bone2 is moved, all bones below it in the heirarchy to be moved too.
bone5 doesnt have children or siblings, so we continue on bone1 with the same algorithm.

*/
void SkinnedModel::BuildToRootMatrices(Bone* pBone, D3DXMATRIX& ParentBoneToRootMatrix) 
{
	//gets the local transformation matrix of the current bone
    D3DXMATRIX BoneLocalMatrix = pBone->TransformationMatrix;

	D3DXMATRIX& ToRootMatrix = pBone->m_toRootMatrix;
	//combines the local transformation matrix of the current bone with the to root matrix of the parent bone
    ToRootMatrix = BoneLocalMatrix * ParentBoneToRootMatrix;

	Bone* pSibling	  = static_cast<Bone*>(pBone->pFrameSibling);
	Bone* pFirstChild = static_cast<Bone*>(pBone->pFrameFirstChild);

	//if this bone has sibling pass the same ParentBoneToRootMatrix as we used in this bone
	if( pSibling )
	{
		BuildToRootMatrices(pSibling, ParentBoneToRootMatrix);
	}

	//if we have child, use the calculated ToRootMatrix
	if( pFirstChild )
	{
		BuildToRootMatrices(pFirstChild, ToRootMatrix);
	}
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::OnResetDevice()
{
	m_pEffect->OnResetDevice();
	m_pTitlesEffect->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::OnLostDevice()
{
	m_pEffect->OnLostDevice();
	m_pTitlesEffect->OnLostDevice();

}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::BuildBoundingBox()
{
	VertexPositionNormalTexture* pVertexBuffer = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);

	D3DXComputeBoundingBox(&pVertexBuffer[0].m_vPos, m_pMesh->GetNumVertices(), sizeof(VertexPositionNormalTexture), &(m_BoundingBox.GetMinPoint()), &(m_BoundingBox.GetMaxPoint()));
	
	m_pMesh->UnlockVertexBuffer();

	
    float width  = m_BoundingBox.GetMaxPoint().x - m_BoundingBox.GetMinPoint().x;
	float height = m_BoundingBox.GetMaxPoint().y - m_BoundingBox.GetMinPoint().y;
	float depth  = m_BoundingBox.GetMaxPoint().z - m_BoundingBox.GetMinPoint().z;
	//fout<<"Bounding box\n"<<width<<endl<<height<<endl<<depth<<endl;

	D3DXCreateBox(pDxDevice, width, height, depth, &m_pBoundingBoxMesh, 0);


	D3DXVECTOR3 center = m_BoundingBox.GetCenter();
	D3DXMatrixTranslation(&(m_BoundingBoxOffset), center.x, center.y, center.z);

	m_BoundingBoxMaterial.m_ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	//below 0.4 does not work...
	m_BoundingBoxMaterial.m_diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.4f);
	m_BoundingBoxMaterial.m_specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_BoundingBoxMaterial.m_fSpecularPower = 8.0f;
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::BuildEffect()
{
	CheckSuccess(D3DXCreateEffectFromFile(pDxDevice, "../../Resources/shaders/SkinnedModelShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, 0));

	m_hEffectTechnique	  = m_pEffect->GetTechniqueByName("SkinnedModelTech");
	m_hWVPMatrix		  = m_pEffect->GetParameterByName(0, "WVP");
	m_hMaterial  		  = m_pEffect->GetParameterByName(0, "mtrl");
	m_hLight	 		  = m_pEffect->GetParameterByName(0, "light");
	m_hTexture	 		  = m_pEffect->GetParameterByName(0, "text");
	m_hFinalBonesMatrices = m_pEffect->GetParameterByName(0, "FinalBonesMatrices");
	m_hIsPicked			  = m_pEffect->GetParameterByName(0, "picked");

	m_pEffect->SetValue(m_hLight, &m_light, sizeof(Light));
	m_pEffect->SetValue(m_hMaterial, &m_whiteMaterial, sizeof(Material));			
}

/////////////////////////////////////////////////////////////////////////

//sets paramateres needed in the effect file for the titles
void SkinnedModel::BuildEffectForTitles()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../Resources/shaders/Text3DShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pTitlesEffect, 0);
	m_hTitlesEffectTechnique = m_pTitlesEffect->GetTechniqueByName("text3DTech");
	m_hTitlesWVPMatrix  	 = m_pTitlesEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::OnUpdate(float dt)
{
	m_pAnimationComponent->OnUpdate(dt, m_movementSpeed);
	
	// Recurse down the tree and builds the toRoot matrix for every bone
	D3DXMATRIX IdentityMatrix;
	D3DXMatrixIdentity(&IdentityMatrix);
	BuildToRootMatrices((Bone*)m_pRoot, IdentityMatrix);

	//every vertex in the skinned mesh is initially in bind space and has to go through these transformations:
	//Bind Space --> (Offset transformation) Bone Space --> (To root transformation) Model Space

	//combines the offset matrix of the frame with the toRoot matrix so the combined matrix
	//can be passed to the shader for rendering
	D3DXMATRIX BoneOffsetMatrix, ToRootMatrix;
	for(UINT i = 0; i < m_nNumBones; ++i)
	{
		BoneOffsetMatrix = *(m_pSkinInfo->GetBoneOffsetMatrix(i));
		ToRootMatrix	 = *(m_vToRootMatrices[i]);

		//we need to multiply by the BoneOffsetMatrix because the vertices in the skinned mesh are not
		//initially according to the space of the bone they are influenced by
		m_vFinalBonesMatrices[i] = BoneOffsetMatrix * ToRootMatrix;
	}
	
	if( m_pGameObjManager->AreObjectsGrounded() &&
		pTerrain->IsValidPosition(m_vPos.x, m_vPos.z) )
	{
		//updates the height of the model, so it can sit above the ground
		m_vPos.y = pTerrain->GetHeight(m_vPos.x,m_vPos.z) + 0.5f;
	}
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::OnRender()
{
	if( m_pGameObjManager->ShouldRenderAxis() )
	{
		D3DXMATRIX R15;

		D3DXVECTOR3 vLook = m_vLook;
		D3DXVECTOR3 vUp = m_vUp;
		D3DXVECTOR3 vRight = m_vRight;
		D3DXVECTOR3 vPos = m_vPos;

		const int nMultiplier = 100*m_fScale;
		vLook*=nMultiplier;
		vUp*=nMultiplier;
		vRight*=nMultiplier;

		D3DXMatrixTranslation(&R15,m_vPos.x,m_vPos.y,m_vPos.z);
		D3DXVec3TransformCoord(&vLook, &vLook, &R15);
		D3DXVec3TransformCoord(&vUp, &vUp, &R15);
		D3DXVec3TransformCoord(&vRight, &vRight, &R15);

		//attempt to center the axis at the center of the model..
		//VertexPositionNormalTexture* pVertexBuffer = 0;
		//m_pMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);

		//D3DXComputeBoundingBox(&pVertexBuffer[0].m_vPos, m_pMesh->GetNumVertices(), sizeof(VertexPositionNormalTexture), &(m_BoundingBox.GetMinPoint()), &(m_BoundingBox.GetMaxPoint()));
		//
		//m_pMesh->UnlockVertexBuffer();

		//D3DXVECTOR3 center = m_BoundingBox.GetCenter();
		//D3DXMatrixTranslation(&(BoundingBoxOffset), center.x, center.y, center.z);
		//D3DXVec3TransformCoord(&vLook, &vLook, &BoundingBoxOffset);
		//D3DXVec3TransformCoord(&vUp, &vUp, &BoundingBoxOffset);
		//D3DXVec3TransformCoord(&vRight, &vRight, &BoundingBoxOffset);
		//D3DXVec3TransformCoord(&vPos, &vPos, &BoundingBoxOffset);

		/*pApp->DrawLine(vPos,vLook);
		pApp->DrawLine(vPos,vUp);
		pApp->DrawLine(vPos,vRight);*/
	}


	D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

	D3DXMATRIX T,S,R,R1,R2;
	D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
	D3DXMatrixRotationX(&R,m_fRotAngleX);
	D3DXMatrixRotationY(&R1,m_fRotAngleY);
	D3DXMatrixRotationZ(&R2,m_fRotAngleZ);

	m_CombinedTransformationMatrix = S*(R*R1*R2)*T;

	m_pEffect->SetTechnique(m_hEffectTechnique);
			
	m_pEffect->SetMatrix(m_hWVPMatrix,&(m_CombinedTransformationMatrix * (camera->GetViewProjMatrix())));
	if( m_pTexture )
	{
		m_pEffect->SetTexture(m_hTexture,m_pTexture);
	}
	else
	{
		m_pEffect->SetTexture(m_hTexture,m_pWhiteTexture);
	}
	
	m_pEffect->SetMatrixArray(m_hFinalBonesMatrices, GetFinalBonesMatricesArray(), m_nNumBones);

	if( m_pGameObjManager->ShouldHighlightPickedObjects() )
	{
		m_pEffect->SetBool(m_hIsPicked,m_bIsPicked);
	}

	UINT numPasses = 0;
	m_pEffect->Begin(&numPasses,0);
	for(UINT i =0;i<numPasses;++i)
	{
		m_pEffect->BeginPass(i);
			m_pMesh->DrawSubset(0);

			if (m_pGameObjManager->ShouldRenderBoundingBoxes())
			{
				RenderBoundingBox();
			}
		m_pEffect->EndPass();
	}
	m_pEffect->End();

	if( m_pGameObjManager->ShouldRenderTitles() )
	{
		RenderTitles();
		RenderTitlesForQuest();
	}
	
	//render the binded objects associated with this skinned mesh
	for (auto& it : m_mapBindedObjects)
	{
		GameObject* pBindedObject = static_cast<StaticModel*>(it.first);
		pBindedObject->RenderBindedWeapon(this, it.second);
	}
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::RenderBindedWeapon(GameObject* pSkMesh, std::string bone)
{


}

/////////////////////////////////////////////////////////////////////////

//renders the titles above the skinned meshes
void SkinnedModel::RenderTitles()
{
	D3DXMATRIX T,S,R,R1;
	D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
	D3DXMatrixRotationX(&R,m_fRotAngleX);
	D3DXMatrixRotationY(&R1,m_fRotAngleY);
	D3DXMATRIX meshCombined = S*(R*R1)*T;

	D3DXMATRIX CombinedMatrix = m_BoundingBoxOffset * meshCombined; 

	AABB boundingBox;
	boundingBox = m_BoundingBox.TransformByMatrix(CombinedMatrix);

	D3DXMatrixTranslation(&T,m_vPos.x,
							 (m_vPos.y + (boundingBox.GetMaxPoint().y - boundingBox.GetMinPoint().y)+5.0f),
							 m_vPos.z);

	D3DXMatrixScaling(&S,m_fScale+3,m_fScale+3,m_fScale+3);

	D3DXMatrixRotationY(&R,m_fTitleRotationAngleByY);

	m_pTitlesEffect->SetTechnique(m_hTitlesEffectTechnique);

	m_pTitlesEffect->SetMatrix(m_hTitlesWVPMatrix, &(S*R*T*camera->GetViewProjMatrix()));
	UINT numPasses = 0;
	m_pTitlesEffect->Begin(&numPasses, 0);
	m_pTitlesEffect->BeginPass(0);

		m_pTitleMesh->DrawSubset(0);

	m_pTitlesEffect->EndPass();
	m_pTitlesEffect->End();
}

/////////////////////////////////////////////////////////////////////////

//renders the titles for quests above the skinned meshes
void SkinnedModel::RenderTitlesForQuest()
{
	if( m_bHasDialogue )
	{
		D3DXMATRIX T,S,R,R1;
		D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
		D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
		D3DXMatrixRotationX(&R,m_fRotAngleX);
		D3DXMatrixRotationY(&R1,m_fRotAngleY);
		D3DXMATRIX meshCombined = S*(R*R1)*T;

		D3DXMATRIX CombinedMatrix = m_BoundingBoxOffset * meshCombined; 

		AABB boundingBox = m_BoundingBox.TransformByMatrix(CombinedMatrix);

		D3DXMatrixTranslation(&T,m_vPos.x,
							  (m_vPos.y + (boundingBox.GetMaxPoint().y - boundingBox.GetMinPoint().y)+10.0f),
							  m_vPos.z);

		D3DXMatrixScaling(&S,m_fScale+10,m_fScale+10,m_fScale+10);

		D3DXMatrixRotationY(&R,m_fTitleForQuestRotationAngleByY);

		m_pTitlesEffect->SetTechnique(m_hTitlesEffectTechnique);

		m_pTitlesEffect->SetMatrix(m_hTitlesWVPMatrix, &(S*R*T*camera->GetViewProjMatrix()));
		UINT numPasses = 0;
		m_pTitlesEffect->Begin(&numPasses, 0);
		m_pTitlesEffect->BeginPass(0);

			m_pTitleForQuestMesh->DrawSubset(0);

		m_pTitlesEffect->EndPass();
		m_pTitlesEffect->End();
	}
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::RenderBoundingBox()
{
	pDxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDxDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDxDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DXMATRIX T,S,R,R1,R2;
	D3DXMatrixTranslation(&T,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixScaling(&S,m_fScale,m_fScale,m_fScale);
	D3DXMatrixRotationX(&R,m_fRotAngleX);
	D3DXMatrixRotationY(&R1,m_fRotAngleY);
	D3DXMatrixRotationZ(&R2, m_fRotAngleZ);
	D3DXMATRIX meshCombined = S*(R*R1*R2)*T;

	m_BoundingBox.m_transformationMatrix = m_BoundingBoxOffset*meshCombined;

	m_pEffect->SetMatrix(m_hWVPMatrix, &(m_BoundingBoxOffset*meshCombined * camera->GetViewProjMatrix()));

	m_pEffect->SetValue(m_hMaterial, &m_BoundingBoxMaterial, sizeof(Material));
	m_pEffect->SetTexture(m_hTexture, m_pWhiteTexture);
	m_pEffect->CommitChanges();
	m_pBoundingBoxMesh->DrawSubset(0);

	m_pEffect->SetValue(m_hMaterial, &m_whiteMaterial, sizeof(Material));	
	pDxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

/////////////////////////////////////////////////////////////////////////


void SkinnedModel::PlayAnimation(LPCSTR strAnimationName)
{
	m_pAnimationComponent->PlayAnimation(strAnimationName);
}

/////////////////////////////////////////////////////////////////////////


void SkinnedModel::PlayAnimationOnceAndStopTrack(LPCSTR strAnimationName)
{
	m_pAnimationComponent->PlayAnimationOnceAndStopTrack(strAnimationName);
}

/////////////////////////////////////////////////////////////////////////

void SkinnedModel::PlayAnimationOnce(LPCSTR strAnimationName)
{
	m_pAnimationComponent->PlayAnimationOnce(strAnimationName);
}

/////////////////////////////////////////////////////////////////////////

bool SkinnedModel::JustStartedPlayingAnimationOnce()
{
	return m_pAnimationComponent->JustStartedPlayingAnimationOnce();
}

/////////////////////////////////////////////////////////////////////////

//Allocates memory for single frame
HRESULT AllocateHierarchy::CreateFrame(PCTSTR Name, D3DXFRAME** NewFrame)
{
	Bone* pBone = new Bone(Name);

	pBone->pMeshContainer = nullptr;
	pBone->pFrameSibling = nullptr;
	pBone->pFrameFirstChild = nullptr;
	D3DXMatrixIdentity(&pBone->TransformationMatrix);
	D3DXMatrixIdentity(&pBone->m_toRootMatrix);

	*NewFrame = pBone;

    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////

//Allocates memory for mesh container. They are used to hold linked list of meshes containers related to one frame
//In skinned meshes there is one mesh only and the linked list of mesh containers isnt used.
HRESULT AllocateHierarchy::CreateMeshContainer(PCTSTR Name, const D3DXMESHDATA* MeshData, const D3DXMATERIAL* Materials, 
											   const D3DXEFFECTINSTANCE* EffectInstances, DWORD NumMaterials, 
											   const DWORD *Adjacency, ID3DXSkinInfo* SkinInfo, D3DXMESHCONTAINER** NewMeshContainer)
{	

    D3DXMESHCONTAINER* pMeshContainer = new D3DXMESHCONTAINER();
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER));	
	if( Name )	
	{
		CopyString(Name, &pMeshContainer->Name);
	}
	else   
	{
		CopyString(nullptr, &pMeshContainer->Name);
	}

	*NewMeshContainer = pMeshContainer;
	
	//if the mesh isnt skinned or it doesnt contain any mesh we dont go on
	if( SkinInfo == nullptr || MeshData->Type != D3DXMESHTYPE_MESH)
	{
		return D3D_OK;
	}


	pMeshContainer->NumMaterials = NumMaterials;
	pMeshContainer->pMaterials   = new D3DXMATERIAL[NumMaterials];
	for(DWORD i = 0; i < NumMaterials; ++i)
	{
		D3DXMATERIAL* mtrls = pMeshContainer->pMaterials;
		mtrls[i].MatD3D =Materials[i].MatD3D;
		mtrls[i].MatD3D.Ambient = Materials[i].MatD3D.Diffuse;

		CopyString(Materials[i].pTextureFilename, &mtrls[i].pTextureFilename);
	}

	pMeshContainer->pEffects   = nullptr;
	pMeshContainer->pAdjacency = nullptr;

	pMeshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
	pMeshContainer->MeshData.pMesh = MeshData->pMesh; 
	pMeshContainer->pSkinInfo      = SkinInfo;
	MeshData->pMesh->AddRef();
	SkinInfo->AddRef();

    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////

//releases the memory allocated for the frame
HRESULT AllocateHierarchy::DestroyFrame(D3DXFRAME* frame) 
{
	delete[] frame->Name;
	delete frame;
 
    return D3D_OK; 
}

/////////////////////////////////////////////////////////////////////////

//releases the memory allocated for mesh container
HRESULT AllocateHierarchy::DestroyMeshContainer(D3DXMESHCONTAINER* MeshContainer)
{
	delete[] MeshContainer->Name;
	delete[] MeshContainer->pAdjacency;
	delete[] MeshContainer->pEffects;
 
	for(DWORD i = 0; i < MeshContainer->NumMaterials; ++i)
	{
		delete[] MeshContainer->pMaterials[i].pTextureFilename;
	}

	delete[] MeshContainer->pMaterials;

	ReleaseX(MeshContainer->MeshData.pMesh);
	ReleaseX(MeshContainer->pSkinInfo);
 
	delete MeshContainer;
 
    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////

float SkinnedModel::GetDistanceToPickedObject()
{
	D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

	GetWorldPickingRay(vOrigin, vDir);

	AABB box = m_BoundingBox.TransformByMatrix(m_CombinedTransformationMatrix);
	if (D3DXBoxBoundProbe(&box.GetMinPoint(), &box.GetMaxPoint(), &vOrigin, &vDir))
	{
		float nDistance = 0.0f;
		D3DXFRAME* pFrame = FindFrameWithMesh(m_pRoot);

		if (CalculateDistanceToPickedObject(pFrame, m_CombinedTransformationMatrix, vOrigin, vDir, nDistance))
		{
			return nDistance;
		}
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////

bool SkinnedModel::CalculateDistanceToPickedObject(D3DXFRAME* pFrame, D3DXMATRIX combinedMatrix, D3DXVECTOR3 vOrigin, D3DXVECTOR3 vDir, float& nDistance)
{
	D3DXMESHCONTAINER* pMeshContainer = pFrame->pMeshContainer;

	D3DXFRAME* pSibling = pFrame->pFrameSibling;
	D3DXFRAME* pFirstChild = pFrame->pFrameFirstChild;

	if (pMeshContainer != NULL)
	{
		//fout<<pFrame->Name << endl;

		D3DXMATRIX InverseWorldMatrix;
		D3DXMatrixInverse(&InverseWorldMatrix, 0, &combinedMatrix);

		//transform the Ray using the inverse matrix
		D3DXVec3TransformCoord(&vOrigin, &vOrigin, &InverseWorldMatrix);
		D3DXVec3TransformNormal(&vDir, &vDir, &InverseWorldMatrix);

		BOOL hit = false;
		DWORD faceIndex = -1;
		float u = 0.0f;
		float v = 0.0f;
		float dist = 0.0f;
		ID3DXBuffer* allhits = nullptr;
		DWORD numHits = 0;

		D3DXIntersect(pMeshContainer->MeshData.pMesh, &vOrigin, &vDir, &hit, &faceIndex, &u, &v, &dist, &allhits, &numHits);
		ReleaseX(allhits);

		nDistance = dist;

		return hit;
	}

	if (pSibling)
	{
		CalculateDistanceToPickedObject(pSibling, combinedMatrix, vOrigin, vDir, nDistance);
	}

	if (pFirstChild)
	{
		CalculateDistanceToPickedObject(pFirstChild, combinedMatrix, vOrigin, vDir, nDistance);
	}

	return false;
}


bool SkinnedModel::IsAttacked() const
{
	return m_bIsAttacked;
}

void SkinnedModel::SetAttacked(bool attacked)
{
	m_bIsAttacked = attacked;
}

bool SkinnedModel::IsAttacking() const
{
	return m_bIsAttacking;
}

void SkinnedModel::SetAttacking(bool attacking)
{
	m_bIsAttacking = attacking;
}

bool SkinnedModel::IsDead() const
{
	return m_bIsDead;
}

void SkinnedModel::SetDead(bool dead)
{
	m_bIsDead = dead;
}

bool SkinnedModel::HasDialogue() const
{
	return m_bHasDialogue;
}

void SkinnedModel::SetHasDialogue(bool hasDialogue)
{
	m_bHasDialogue = hasDialogue;
}

std::string SkinnedModel::GetAttackerName() const
{
	return m_strAttackerName;
}

void SkinnedModel::SetAttackerName(const std::string& attackerName)
{
	m_strAttackerName = attackerName;
}

std::string SkinnedModel::GetActorType() const
{
	return m_strActorType;
}

void SkinnedModel::SetActorType(const std::string& actorType)
{
	m_strActorType = actorType;
}

ID3DXMesh** SkinnedModel::GetTitleMesh()
{
	return&m_pTitleMesh;
}

void SkinnedModel::SetTitleMesh(ID3DXMesh* titleMesh)
{
	m_pTitleMesh = titleMesh;
}

D3DXVECTOR3& SkinnedModel::GetTitleLookVector()
{
	return m_vTitleLook;
}

D3DXVECTOR3& SkinnedModel::GetTitleRightVector()
{
	return m_vTitleRight;
}

D3DXVECTOR3& SkinnedModel::GetTitleUpVector()
{
	return m_vTitleUp;
}

D3DXMATRIX SkinnedModel::GetTitleRotationMatrix() const
{
	return m_TitleRotationMatrix;
}

void SkinnedModel::SetTitleRotationMatrix(D3DXMATRIX titleRotationMatrix)
{
	m_TitleRotationMatrix = titleRotationMatrix;
}

float SkinnedModel::GetTitleRotationAnglyByY() const
{
	return m_fTitleRotationAngleByY;
}

void SkinnedModel::SetTitleRotationAnglyByY(float angle)
{
	m_fTitleRotationAngleByY = angle;
}

void SkinnedModel::ModifyTitleRotationAnglyByY(float delta)
{
	m_fTitleRotationAngleByY += delta;
}

std::string SkinnedModel::GetTitleForQuest() const
{
	return m_strTitleForQuest;
}

void SkinnedModel::SetTitleForQuest(const std::string& titleForQuest)
{
	m_strTitleForQuest = titleForQuest;
}

ID3DXMesh** SkinnedModel::GetTitleForQuestMesh()
{
	return &m_pTitleForQuestMesh;
}

void SkinnedModel::SetTitleForQuestMesh(ID3DXMesh* titleForQuestMesh)
{
	m_pTitleForQuestMesh = titleForQuestMesh;
}

D3DXVECTOR3& SkinnedModel::GetTitleForQuestLookVector()
{
	return m_vTitleForQuestLook;
}

D3DXVECTOR3& SkinnedModel::GetTitleForQuestRightVector()
{
	return m_vTitleForQuestRight;
}

D3DXVECTOR3& SkinnedModel::GetTitleForQuestUpVector()
{
	return m_vTitleForQuestUp;
}

D3DXMATRIX SkinnedModel::GetTitleForQuestRotationMatrix() const
{
	return m_TitleForQuestRotationMatrix;
}

void SkinnedModel::SetTitleForQuestRotationMatrix(D3DXMATRIX titleForQuestRotationMatrix)
{
	m_TitleForQuestRotationMatrix = titleForQuestRotationMatrix;
}

float SkinnedModel::GetTitleForQuestRotationAnglyByY() const
{
	return m_fTitleForQuestRotationAngleByY;
}

void SkinnedModel::SetTitleForQuestRotationAnglyByY(float angle)
{
	m_fTitleForQuestRotationAngleByY = angle;
}

void SkinnedModel::ModifyTitleForQuestRotationAnglyByY(float delta)
{
	m_fTitleForQuestRotationAngleByY += delta;
}

std::vector<D3DXMATRIX>& SkinnedModel::GetFinalBonesMatrices()
{
	return m_vFinalBonesMatrices;
}

void SkinnedModel::SetFinalBonesMatrices(std::vector<D3DXMATRIX> bonesMatrices)
{
	m_vFinalBonesMatrices = bonesMatrices;
}

std::vector<D3DXMATRIX*>& SkinnedModel::GetToRootMatrices()
{
	return m_vToRootMatrices;
}

void SkinnedModel::SetToRootMatrices(std::vector<D3DXMATRIX*> toRootMatrices)
{
	m_vToRootMatrices = toRootMatrices;
}

D3DXFRAME* SkinnedModel::GetRootFrame() const
{
	return m_pRoot;
}

void SkinnedModel::SetRootFrame(D3DXFRAME* rootFrame)
{
	m_pRoot = rootFrame;
}

DWORD SkinnedModel::GetMaxVertexInfluences() const
{
	return m_nMaxVertInfluences;
}

void SkinnedModel::SetMaxVertexInfluences(DWORD maxVertexInfluences)
{
	m_nMaxVertInfluences = maxVertexInfluences;
}

DWORD SkinnedModel::GetAmountOfBones() const
{
	return m_nNumBones;
}

void SkinnedModel::SetAmountOfBones(DWORD amountOfBones)
{
	m_nNumBones = amountOfBones;
}

ID3DXSkinInfo* SkinnedModel::GetSKinInfo() const
{
	return m_pSkinInfo;
}

void SkinnedModel::SetSkinInfo(ID3DXSkinInfo* skinInfo)
{
	m_pSkinInfo = skinInfo;
}

void SkinnedModel::SetMovementSpeed(float newSpeed)
{
	m_movementSpeed = newSpeed;
}

float SkinnedModel::GetMovementSpeed()
{
	return m_movementSpeed;
}