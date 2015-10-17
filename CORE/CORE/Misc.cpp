#include"Misc.h"
#include"Camera.h"
#include"TextManager.h"
#include <string.h>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Function:getWorldPickingRay
Purpose:transforms ray from screen space to world space, used for picking models in the game with the mouse
*/
void GetWorldPickingRay(D3DXVECTOR3& vOrigin, D3DXVECTOR3& vDirection)
{
	//TR level:Picking
	POINT pCursorPosition;
	GetCursorPos(&pCursorPosition);

	//transforms the position of the cursor to be relative to the client's window
	ScreenToClient(pApp->GetMainWindow(), &pCursorPosition);

	float w = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth);
	float h = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight);

	D3DXMATRIX ProjMatrix = camera->GetProjMatrix();

	//transforms from screen space to view space
	float x = (2.0f * pCursorPosition.x/w - 1.0f) / ProjMatrix(0,0);
	float y = (-2.0f*pCursorPosition.y/h + 1.0f) / ProjMatrix(1,1);

	//so far the cursor position is transformed from screen space to view space
	//our models are in world space, though, so we must make one more transformation - from view space to world space
	D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 direction(x, y, 1.0f);

	//makes inverse of the view matrix, to transform the coordinates to world space
	D3DXMATRIX InverseViewMatrix;
	D3DXMatrixInverse(&InverseViewMatrix, 0, &(camera->GetViewMatrix()));

	D3DXVec3TransformCoord(&vOrigin, &origin, &InverseViewMatrix);
	D3DXVec3TransformNormal(&vDirection, &direction, &InverseViewMatrix);
	D3DXVec3Normalize(&vDirection, &vDirection);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Function:getWidth
Purpose:gets width of text when drawn on screen
*/
float GetStringWidth(string strString)
{
	RECT rStringInfo;
	ZeroMemory(&rStringInfo, sizeof(RECT));

	if( pTextManager->GetFont2D() != NULL )
	{
		pTextManager->GetFont2D()->DrawTextA(NULL, strString.c_str(), strString.length(),&rStringInfo, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));
	}
	return static_cast<float>(rStringInfo.right);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Function:getHeight
Purpose:gets height of text when drawn on screen
*/
float GetStringHeight(string strString)
{
	RECT rStringInfo;
	ZeroMemory(&rStringInfo, sizeof(RECT));

	if( pTextManager->GetFont2D() != NULL )
	{
		pTextManager->GetFont2D()->DrawTextA(NULL, strString.c_str(), strString.length(),&rStringInfo, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));
	}
	return static_cast<float>(rStringInfo.bottom);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Function:hasNormals()
Purpose:checks if the mesh got normals
*/
bool HasNormals(ID3DXMesh* pMesh)
{
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(elems);
	
	bool bHasNormals = false;

	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		//if we reached the end of the declaration
		if(elems[i].Stream == 0xff)
		{
			break;
		}

		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
			elems[i].UsageIndex == 0 )
		{
			bHasNormals = true;
			break;
		}
	}

	return bHasNormals;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Function:InitVertexDeclarations
Purpose:initialize vertex declarations. For now only position, normal, texture declaration is used
		and only it is initalized and saved in the GamepApp, so we can access it later.
*/
void InitVertexDeclarations()
{
	//position only
	D3DVERTEXELEMENT9 vertexPositionElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};	

	//position and color
	D3DVERTEXELEMENT9 vertexPositionColorElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};	

	//position and normal
	D3DVERTEXELEMENT9 vertexPositionNormalElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};	

	//position and texture
	D3DVERTEXELEMENT9 vertexPositionTextureElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	

	//position,normal and texture
	D3DVERTEXELEMENT9 vertexPositionNormalTextureElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	IDirect3DVertexDeclaration9* pDecl = NULL;
	pDxDevice->CreateVertexDeclaration(vertexPositionNormalTextureElements, &pDecl);
	pApp->SetPositionNormalTextureDecl(pDecl);

	pDecl = NULL;
	pDxDevice->CreateVertexDeclaration(vertexPositionColorElements, &pDecl);
	pApp->SetPosColDeclaration(pDecl);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Function:CopyString
Purpose:this function is used because the name which is passed to create frame is PCTSTR and the name in the frame is LPCSTR
		and they cannot be casted.
*/
void CopyString(const char* input, char** output)
{
	if(input)
	{
		int length = strlen(input) + 1;
		*output = new char[length];
		strcpy_s(*output, length, input);
	}
	else
	{
		*output = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////