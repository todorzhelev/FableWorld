#include"SkinnedMesh.h"
#include"StaticMesh.h"


/////////////////////////////////////////////////////////////////////////
/*
Function:SkinnedMesh
Purpose:constructor
*/
SkinnedMesh::SkinnedMesh()
:m_fTransTimeForAttack(0.125f)
,m_fTransTimeForIdle(0.25f)
{
	//default white texture for models which doesnt have any
	D3DXCreateTextureFromFile(pDxDevice, "../../Resources/textures/whitetex.dds", &m_pWhiteTexture);

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

	BuildEffect();
	BuildEffectForTitles();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:SkinnedMesh
Purpose:this constructor loads default values for the skinned mesh
*/
SkinnedMesh::SkinnedMesh(string strModelName, string ModelFileName, string strTextureFileName,bool bShouldRenderTitles)
:m_fTransTimeForAttack(0.125f)
,m_fTransTimeForIdle(0.25f)
{
	//code duplication, move it to another function
	//default white texture for models which doesnt have any
	D3DXCreateTextureFromFile(pDxDevice, "../../Resources/textures/whitetex.dds", &m_pWhiteTexture);

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
	m_nCurrentAnimTrack = 0;
	m_nNewAnimTrack = 1;
	m_bIsSwitched = false;
	m_bIsDead = false;
	m_bIsPicked = false;
	m_bHasDialogue = false;
	m_strAttackerName = "";

	m_eGameObjectType = EGameObjectType_Skinned;

	m_bShouldRenderTitles = bShouldRenderTitles;
}
/////////////////////////////////////////////////////////////////////////
/*
Function:LoadGameObject
Purpose:loads the animated model
*/
void SkinnedMesh::LoadGameObject()
{
	//creates the texture for the model
	D3DXCreateTextureFromFile(pDxDevice, m_strTextureFileName.c_str(), &m_pTexture);

	AllocateHierarchy allocMeshHierarchy;

	if( FAILED( D3DXLoadMeshHierarchyFromX(m_strModelFileName.c_str(), D3DXMESH_SYSTEMMEM,pDxDevice, &allocMeshHierarchy, 0,&m_pRoot,&m_pAnimController) ) )
	{
		fout<<"cannot load animated model. filename:"<<m_strModelFileName.c_str()<<endl;
		return;
	}

	//finds the bone that holds the only mesh.In skinned meshes there is just one model
	//which is controlled by the bones hiearchy
	D3DXFRAME* pFrame = FindFrameWithMesh(m_pRoot);

	if( !pFrame ) 
	{
		fout<<"cannot find node with mesh"<<endl;
	}
	
	D3DXMESHCONTAINER* pMeshContainer = pFrame->pMeshContainer;
	m_pSkinInfo = pMeshContainer->pSkinInfo;

	m_nNumBones = pMeshContainer->pSkinInfo->GetNumBones();
	m_vFinalBonesMatrices.resize(m_nNumBones);
	m_vToRootMatrices.resize(m_nNumBones, 0);

	char str[80];
	strcpy_s(str,"../../Resources/textures/");
	strcat_s(str,pMeshContainer->pMaterials->pTextureFilename);

	m_strTextureFileName = str;

	//creates the texture for the model
	D3DXCreateTextureFromFile(pDxDevice, m_strTextureFileName.c_str(), &m_pTexture);

	//set the idle animation on the current track and mantain one more track, 
	//which we will use later to switch to attack or dead animations
	m_pAnimController->SetTrackEnable( m_nNewAnimTrack, true ); 
	m_pAnimController->SetTrackEnable( m_nCurrentAnimTrack, true );
	m_pAnimController->GetAnimationSetByName("idle",&m_pCurrentAnimSet);
	m_pAnimController->SetTrackAnimationSet(m_nCurrentAnimTrack,m_pCurrentAnimSet);
	m_pAnimController->KeyTrackSpeed( m_nCurrentAnimTrack, 1.0f, 0.0f, m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );  
	m_pAnimController->KeyTrackWeight( m_nCurrentAnimTrack, 1.0f, 0.0f, m_fTransTimeForAttack, D3DXTRANSITION_LINEAR ); 
 
	m_pAnimController->KeyTrackSpeed( m_nNewAnimTrack, 0.0f, 0.0f, m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );  
	m_pAnimController->KeyTrackWeight( m_nNewAnimTrack, 0.0f, 0.0f, m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );
	
	//after the mesh is found it is then changed in order to become skinned mesh
	BuildSkinnedMesh(pMeshContainer->MeshData.pMesh);

	BuildToRootMatricesPtrArray();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:~SkinnedMesh
Purpose:destructor
*/
SkinnedMesh::~SkinnedMesh()
{
}

/////////////////////////////////////////////////////////////////////////
/*
Function:GetFinalBonesMatricesArray
Purpose:returns the final matrices for each bone which are passed to the shader for rendering.
		We are returning the adress of the first element of the array, because in the shader we need the beginning of the array and the size of it
*/
D3DXMATRIX* SkinnedMesh::GetFinalBonesMatricesArray()
{
	return &m_vFinalBonesMatrices[0];
}

/////////////////////////////////////////////////////////////////////////
/*
Function:PlayAnimation()
Purpose:sets the animation index on the mesh at the current track. 
This function repeats the animation, i.e. after the animation set ended it starts from the beginning and so on.
*/
void SkinnedMesh::PlayAnimation(LPCSTR strAnimationName)
{
	m_pAnimController->GetAnimationSetByName(strAnimationName,&m_pCurrentAnimSet);
	m_pAnimController->SetTrackAnimationSet(m_nCurrentAnimTrack,m_pCurrentAnimSet);
}

/////////////////////////////////////////////////////////////////////////
/*
Function:playAnimationOnceAndStop()
Purpose:plays animation only once then stops( used for dead animation )
The idea is to slightly move from the current track to the new one, holding the new animation.
After the new animation is played to the end, we stop it. 
This way the two animation tracks are with 0.0 weight and thus no animation is played.
*/
void SkinnedMesh::PlayAnimationOnceAndStop(LPCSTR strAnimationName)
{
	m_pAnimController->GetAnimationSetByName(strAnimationName,&m_pSecondAnimSet);
	m_pAnimController->SetTrackAnimationSet(m_nNewAnimTrack,m_pSecondAnimSet);

	m_pAnimController->KeyTrackSpeed( m_nCurrentAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_nCurrentAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );
	
	m_pAnimController->KeyTrackSpeed( m_nNewAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );  
	m_pAnimController->KeyTrackWeight( m_nNewAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );

	D3DXTRACK_DESC td;
	m_pAnimController->GetTrackDesc(m_nNewAnimTrack,&td);
	if( td.Position >= m_pSecondAnimSet->GetPeriod() - 0.35 )
	{ 
		m_pAnimController->KeyTrackSpeed( m_nNewAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR );  
		m_pAnimController->KeyTrackWeight( m_nNewAnimTrack, 0.1f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR );
	}

}

/////////////////////////////////////////////////////////////////////////
/*
Function:playAnimationOnce()
Purpose:plays animation only once.switches to idle animation( used for transition between idle and attack )
rand is used so it can be randomly determined which one from the 2 attack animations to be played.
*/
void SkinnedMesh::PlayAnimationOnce(LPCSTR strAnimationName)
{
		srand(static_cast<unsigned int>(time(NULL)));

		int i = rand()%2+1;
		if( i==2 )
		{
			m_pAnimController->GetAnimationSetByName("attack_2",&m_pSecondAnimSet);
			m_pAnimController->SetTrackAnimationSet(m_nNewAnimTrack,m_pSecondAnimSet);
		}
		else if( i==1 )
		{
			m_pAnimController->GetAnimationSetByName("attack_1",&m_pSecondAnimSet);
			m_pAnimController->SetTrackAnimationSet(m_nNewAnimTrack,m_pSecondAnimSet);
		}

		//switches from the current animation for instance idle to the new one which must be played once only
		D3DXTRACK_DESC td;
		m_pAnimController->GetTrackDesc(m_nNewAnimTrack,&td);
		if( !m_bIsSwitched )
		{
			m_pAnimController->KeyTrackSpeed( m_nCurrentAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );
			m_pAnimController->KeyTrackWeight( m_nCurrentAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );

			m_pAnimController->KeyTrackSpeed( m_nNewAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );  
			m_pAnimController->KeyTrackWeight( m_nNewAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForAttack, D3DXTRANSITION_LINEAR );

			m_bIsSwitched = true;
			m_bIsAttacking = true;
			//with this code we just transit from idle to attack. At this moment the attack animation is played, but if the player
			//releases the mouse button, the attack animation must be switched to idle.
			//so we wait till the animaiton is over and next part that switches from attack to idle is finished in onUpdate
		}
		else 
		{
			m_bIsAttacking = false;
		}
	
}

/////////////////////////////////////////////////////////////////////////
/*
Function:FindFrameWithMesh
Purpose:finds the frame that contains mesh
*/
D3DXFRAME* SkinnedMesh::FindFrameWithMesh(D3DXFRAME* frame)
{
	if( frame->pMeshContainer )
	{
		if( frame->pMeshContainer->MeshData.pMesh != NULL )
		{
			return frame;
		}
	}

	D3DXFRAME* pFrame = NULL;
	if(frame->pFrameSibling)
	{
		if( pFrame = FindFrameWithMesh(frame->pFrameSibling) )	
		{
			return pFrame;
		}
	}

	if(frame->pFrameFirstChild)
	{
		if( pFrame = FindFrameWithMesh(frame->pFrameFirstChild) )
		{
			return pFrame;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:buildSkinnedMesh()
Purpose:creates the mesh and converts it to skinned mesh and also computes normals for the mesh if needed
*/
void SkinnedMesh::BuildSkinnedMesh(ID3DXMesh* pMesh)
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
Function:BuildToRootMatricesPtrArray()
Purpose:Stores pointers to the to root matrices of all the bones in the skinned mesh
		The to root matrices will be updated every frame in OnUpdate, and we will have
		quick access to these matrices just by index, instead of using D3DXFrameFind every time in OnUpdate
		These matrices are needed to calculate the final matrix transformation for the vertices
*/
void SkinnedMesh::BuildToRootMatricesPtrArray()
{
	for(UINT i = 0; i < m_nNumBones; ++i)
	{
		const char* boneName = m_pSkinInfo->GetBoneName(i);
		D3DXFRAME* pFrame	 = D3DXFrameFind(m_pRoot, boneName);
		
		if( pFrame )
		{
			FrameEx* pFrameEx = static_cast<FrameEx*>( pFrame );
			m_vToRootMatrices[i] = &pFrameEx->ToRootMatrix;
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/*
Function:bindWeaponToModel()
Purpose:binds static object to animated model's bone
*/
void SkinnedMesh::BindWeaponToModel(string strObjectName,string strBoneToBind)
{
	GameObject* pGameObject = m_pGameObjManager->GetGameObjects().find(strObjectName.c_str())->second;
	m_mapBindedObjects[pGameObject] = strBoneToBind;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:BuildToRootMatrices()
Purpose:Traverse the whole ierarchy of frames and builds the to root matrices for every frame
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
void SkinnedMesh::BuildToRootMatrices(FrameEx* pBone, D3DXMATRIX& ParentBoneToRootMatrix) 
{
	//gets the local transformation matrix of the current bone
    D3DXMATRIX BoneLocalMatrix = pBone->TransformationMatrix;

	D3DXMATRIX& ToRootMatrix = pBone->ToRootMatrix;
	//combines the local transformation matrix of the current bone with the to root matrix of the parent bone
    ToRootMatrix = BoneLocalMatrix * ParentBoneToRootMatrix;

    FrameEx* pSibling	 = (FrameEx*)pBone->pFrameSibling;
    FrameEx* pFirstChild = (FrameEx*)pBone->pFrameFirstChild;

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
/*
Function:OnResetDevice
Purpose:restores the effect file from onLostDevice status
*/
void SkinnedMesh::OnResetDevice()
{
	m_pEffect->OnResetDevice();
	m_pTitlesEffect->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:this can occur if alt+tab is pressed
*/
void SkinnedMesh::OnLostDevice()
{
	m_pEffect->OnLostDevice();
	m_pTitlesEffect->OnLostDevice();

}

/////////////////////////////////////////////////////////////////////////
/*
Function:BuildBoundingBox
Purpose:builds the bounding box around the model. Used for collision detection tests and visible tests
*/
void SkinnedMesh::BuildBoundingBox()
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

	m_BoundingBoxMaterial.m_ambient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_diffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.17f);
	m_BoundingBoxMaterial.m_fSpecularPower = 8.0f;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:BuildEffect
Purpose:sets paramateres needed in the effect file
*/
void SkinnedMesh::BuildEffect()
{
	if(FAILED(D3DXCreateEffectFromFile(pDxDevice,"../../CORE/CORE/shaders/SkinnedMeshShader.fx",0,0,D3DXSHADER_DEBUG,0,&m_pEffect,0)))
	{
		MessageBox(0,"Failed loading effect file in skinnedMesh",0,0);
		PostQuitMessage(0);
	}

	m_hEffectTechnique	  = m_pEffect->GetTechniqueByName("SkinnedMeshTech");
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
/*
Function:BuildEffectForTitles
Purpose:sets paramateres needed in the effect file for the titles
*/
void SkinnedMesh::BuildEffectForTitles()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../CORE/CORE/shaders/Text3DShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pTitlesEffect, 0);
	m_hTitlesEffectTechnique = m_pTitlesEffect->GetTechniqueByName("text3DTech");
	m_hTitlesWVPMatrix  	 = m_pTitlesEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate()
Purpose:updates the mesh bones and current position
*/
void SkinnedMesh::OnUpdate(float fDeltaTime)
{
	//this if is the second part of the algorithm for playing animation just once.
	//after the function for playing animation once starts playing the next animation(for instance we play attack animation)
	//we need time so the attack animation finishes and we can transit to idle animation again.
	//thus we need a function that is invoked every frame and therefore playAnimationOnce won't do the job,
	//since its invoked only when the mouse button is down.
	if( m_bIsSwitched )
	{
		D3DXTRACK_DESC td;
		m_pAnimController->GetTrackDesc(m_nNewAnimTrack,&td);
		//after the attack animation has finished we slightly make transition to idle animation.
		if( td.Position >= m_pSecondAnimSet->GetPeriod() )
		{ 
				m_pAnimController->KeyTrackSpeed( m_nNewAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR );  
				m_pAnimController->KeyTrackWeight( m_nNewAnimTrack, 0.0f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR ); 

				m_pAnimController->KeyTrackSpeed( m_nCurrentAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR );  
				m_pAnimController->KeyTrackWeight( m_nCurrentAnimTrack, 1.0f, m_pAnimController->GetTime(), m_fTransTimeForIdle, D3DXTRANSITION_LINEAR );
				m_bIsSwitched = false;
				m_pAnimController->SetTrackPosition( m_nNewAnimTrack,0.0);
		}
	}

	m_pAnimController->AdvanceTime(fDeltaTime, NULL);
	
	// Recurse down the tree and builds the toRoot matrix for every bone
	D3DXMATRIX IdentityMatrix;
	D3DXMatrixIdentity(&IdentityMatrix);
	BuildToRootMatrices((FrameEx*)m_pRoot, IdentityMatrix);

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
	
	if( m_pGameObjManager->AreObjectsGrounded() )
	{
		//updates the height of the model, so it can sit above the ground
		m_vPos.y = pTerrain->GetHeight(m_vPos.x,m_vPos.z) + 0.5f;
	}
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:renders the skinned mesh on the screen
*/
void SkinnedMesh::OnRender()
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

		pApp->DrawLine(vPos,vLook);
		pApp->DrawLine(vPos,vUp);
		pApp->DrawLine(vPos,vRight);
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
			RenderBoundingBox();
		m_pEffect->EndPass();
	}
	m_pEffect->End();

	if( m_pGameObjManager->ShouldRenderTitles() )
	{
		RenderTitles();
		RenderTitlesForQuest();
	}
	
	for (auto& it : m_mapBindedObjects)
	{
		GameObject* pBindedObject = static_cast<StaticMesh*>(it.first);
		pBindedObject->RenderBindedWeapon(this, it.second);
	}
}

/////////////////////////////////////////////////////////////////////////

void SkinnedMesh::RenderBindedWeapon(GameObject* pSkMesh, string bone)
{


}

/////////////////////////////////////////////////////////////////////////
/*
Function:RenderTitles
Purpose:renders the titles above the skinned meshes
*/
void SkinnedMesh::RenderTitles()
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
/*
Function:renderTitlesForQuest
Purpose:renders the titles for quests above the skinned meshes
*/
void SkinnedMesh::RenderTitlesForQuest()
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
/*
Function:RenderBoundingBox
Purpose:renders the bounding box
*/
void SkinnedMesh::RenderBoundingBox()
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

	m_pEffect->SetMatrix(m_hWVPMatrix, &(m_BoundingBoxOffset * meshCombined * camera->GetViewProjMatrix()));

	m_pEffect->SetValue(m_hMaterial, &m_BoundingBoxMaterial, sizeof(Material));
	m_pEffect->SetTexture(m_hTexture, m_pWhiteTexture);
	m_pEffect->CommitChanges();
	m_pBoundingBoxMesh->DrawSubset(0);

	m_pEffect->SetValue(m_hMaterial, &m_whiteMaterial, sizeof(Material));	
	pDxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

/////////////////////////////////////////////////////////////////////////
/*
Function:CreateFrame
Purpose:Allocates memory for single frame
*/
HRESULT AllocateHierarchy::CreateFrame(PCTSTR Name, D3DXFRAME** NewFrame)
{
	FrameEx* pFrameEx = new FrameEx();

	if( Name )	
	{
		CopyString(Name, &pFrameEx->Name);
	}
	else	
	{
		CopyString(NULL, &pFrameEx->Name);
	}

	pFrameEx->pMeshContainer = 0;
	pFrameEx->pFrameSibling = 0;
	pFrameEx->pFrameFirstChild = 0;
	D3DXMatrixIdentity(&pFrameEx->TransformationMatrix);
	D3DXMatrixIdentity(&pFrameEx->ToRootMatrix);

	*NewFrame = pFrameEx;

    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:CreateMeshContainer
Purpose:Allocates memory for mesh container. They are used to hold linked list of meshes containers related to one frame
		In skinned meshes there is one mesh only and the linked list of mesh containers isnt used.
*/
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
		CopyString(NULL, &pMeshContainer->Name);
	}

	*NewMeshContainer = pMeshContainer;
	
	//if the mesh isnt skinned or it doesnt contain any mesh we dont go on
	if( SkinInfo == 0 || MeshData->Type != D3DXMESHTYPE_MESH)
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

	pMeshContainer->pEffects   = 0;
	pMeshContainer->pAdjacency = 0;

	pMeshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
	pMeshContainer->MeshData.pMesh = MeshData->pMesh; 
	pMeshContainer->pSkinInfo      = SkinInfo;
	MeshData->pMesh->AddRef();
	SkinInfo->AddRef();

    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:DestroyFrame
Purpose:releases the memory allocated for the frame
*/
HRESULT AllocateHierarchy::DestroyFrame(D3DXFRAME* frame) 
{
	delete[] frame->Name;
	delete frame;
 
    return D3D_OK; 
}

/////////////////////////////////////////////////////////////////////////
/*
Function:DestroyMeshContainer
Purpose:releases the memory allocated for mesh container
*/
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

	releaseX(MeshContainer->MeshData.pMesh);
	releaseX(MeshContainer->pSkinInfo);
 
	delete MeshContainer;
 
    return D3D_OK;
}

/////////////////////////////////////////////////////////////////////////