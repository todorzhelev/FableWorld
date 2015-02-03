#include"Button3D.h"

/*
SourceFile: Button3D.cpp
Purpose: Contains functions for loading and rendering buttons
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Init
Purpose:creates to sprite for rendering the button and loads the textures of the button:
		the mouseover texture and normal texture
*/
void Button3D::Init()
{
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);
	m_bIsMouseOver = true;
	m_bIsMouseDown = true;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:invokes the OnLostDevice function of the sprite 
*/
void Button3D::OnLostDevice()
{
	//m_pSprite->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:invokes the OnResetDevice function of the sprite 
*/
void Button3D::OnResetDevice()
{
	//m_pSprite->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseOver
Purpose:checks if the mouse is over the texture
*/
bool Button3D::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	//ScreenToClient makes the mouse coordinates relative to area of the program not to the screen 
	//i.e. the coordinates will start from top-left position of the window
	ScreenToClient(pApp->GetMainWindow(), &s);

	//must do it for 3d
		
	if( m_pBackgroundMesh->m_bIsPicked )
	{
		return true;		
	}
	else
	{
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseDown
Purpose:if the mouse is over the image and the mouse button is clicked we got mouse down event
*/
bool Button3D::IsMouseDown()
{
	//if the mouse is over the area and the left button is pressed then return true
	if(IsMouseOver() && pDinput->IsMouseButtonDown(0))
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:IsClicked
Purpose:if the button is clicked
*/
bool Button3D::IsClicked()
{
	return m_bIsMouseDown;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnUpdate
Purpose:updates the variables of the guiobject
*/
void Button3D::OnUpdate()
{
	if(IsMouseOver())
	{
		m_bIsMouseOver = true;
	}
	else
	{
		m_bIsMouseOver = false;
	}

	if(IsMouseDown())
	{
		m_bIsMouseDown = true;
	}
	else 
	{   
		m_bIsMouseDown = false;
	}

	//m_pBackgroundMesh->OnUpdate();
}

bool Button3D::IsPicked(D3DXMATRIX TransformationMatrix,D3DXVECTOR3 origin,D3DXVECTOR3 dir)
{
	AABB box;
	bool bIsPicked = false;
	
	m_BoundingBox.transformByMatrix(TransformationMatrix, box);
	bIsPicked = D3DXBoxBoundProbe(&box.minPt, &box.maxPt, &origin, &dir);
	
	return bIsPicked;
}

void Button3D::BuildBoundingBox()
{
	//D3DVERTEXELEMENT9 elements[64];
	//UINT numElements = 0;

	//pApp->GetPosColDeclaration()->GetDeclaration(elements, &numElements);

	//ID3DXMesh* pTempMesh = 0;
	//m_pTextMesh->CloneMesh(D3DXMESH_SYSTEMMEM, elements, pDxDevice, &pTempMesh);

	//VertexPositionColor* pVertexBuffer = 0;
	//pTempMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);



	//D3DXComputeBoundingBox(&pVertexBuffer[0].m_vPos, pTempMesh->GetNumVertices(), sizeof(VertexPositionColor), &(m_BoundingBox.minPt), &(m_BoundingBox.maxPt));
	//
	//pTempMesh->UnlockVertexBuffer();
	//
	//float width  = m_BoundingBox.maxPt.x - m_BoundingBox.minPt.x;
	//float height = m_BoundingBox.maxPt.y - m_BoundingBox.minPt.y;
	//float depth  = m_BoundingBox.maxPt.z - m_BoundingBox.minPt.z;

	//fout<<"Bounding box\n"<<width<<endl<<height<<endl<<depth<<endl;

	/*D3DXCreateBox(pDxDevice, width, height, depth, &m_pBoundingBoxMesh, 0);

	D3DXVECTOR3 center = m_BoundingBox.center();
	D3DXMatrixTranslation(&(m_BoundingBoxOffset), center.x, center.y, center.z);

	m_BoundingBoxMaterial.m_ambient  		= D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_diffuse   		= D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.17f);
	m_BoundingBoxMaterial.m_specular     	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.17f);
	m_BoundingBoxMaterial.m_fSpecularPower 	= 8.0f;*/
}
/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:render the button
*/
void Button3D::OnRender()
{
	m_pBackgroundMesh->m_vPos = D3DXVECTOR3(0,0,50);
	m_pBackgroundMesh->OnRender();

	D3DXMATRIX T,S,R;

	D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

	float fDifference = 3.5f;

	D3DXMatrixTranslation(&T,-9,+4,50+fDifference);

	//D3DXMatrixScaling(&S,0.5,0.5,0.5);

	m_pShaderEffect->SetTechnique(m_hShaderTechnique);

	m_pShaderEffect->SetMatrix(m_hShaderWVPMatrix, &(T*camera->GetViewProjMatrix()));
	UINT numPasses = 0;
	m_pShaderEffect->Begin(&numPasses, 0);
	m_pShaderEffect->BeginPass(0);

		m_pTextMesh->DrawSubset(0);

		//check for picking
		if( pDinput->IsMouseButtonDown(0) )
		{
			GetWorldPickingRay(vOrigin, vDir);

			if( IsPicked(S*T,vOrigin,vDir) )
			{
				////by finding the inverse of mesh CombinedMatrix we transform from World space to the mesh local space
				////this is needed, because we have to transform the picking vectors to the mesh local space
				////which is required by the D3DXIntersect function
				//D3DXMATRIX InverseWorldMatrix;
				//D3DXMatrixInverse(&InverseWorldMatrix, 0, &(S*T));

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
				//D3DXIntersect(m_pTextMesh, &vOrigin, &vDir, &hit,&faceIndex, &u, &v, &dist, &allhits, &numHits);
				//release(allhits);

				//if( hit )
				{
					m_bIsPicked = true;
				}		
			}
		}

	m_pShaderEffect->EndPass();
	m_pShaderEffect->End();
}


/////////////////////////////////////////////////////////////////////////