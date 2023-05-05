#include <stdafx.h>
#include "Misc.h"
#include "Camera.h"

//checks if the mesh got normals
bool HasNormals(ID3DXMesh* pMesh) {
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(elems);
	
	bool bHasNormals = false;

	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i) {
		//if we reached the end of the declaration
		if(elems[i].Stream == 0xff) {
			break;
		}
		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
			elems[i].UsageIndex == 0 ) {
			bHasNormals = true;
			break;
		}
	}
	return bHasNormals;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

//initialize vertex declarations. For now only position, normal, texture declaration is used
//and only it is initalized and saved in the GamepApp, so we can access it later.
void InitVertexDeclarations() {
	//position only (12B size)
	D3DVERTEXELEMENT9 vP[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};	

	//position and color (28B)
	D3DVERTEXELEMENT9 vPC[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};	

	//position and normal (24B size)
	D3DVERTEXELEMENT9 vPN[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};	

	//position and texture (20B size)
	D3DVERTEXELEMENT9 vPT[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	

	//position,normal and texture (32B size)
	D3DVERTEXELEMENT9 vPNT[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	//56B. D3DCOLOR is 4x4B floats
	D3DVERTEXELEMENT9 vPs[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		{ 0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	IDirect3DVertexDeclaration9* pDecl = NULL;
	pDxDevice->CreateVertexDeclaration(vPNT, &pDecl);
	pApp->SetPNTDecl(pDecl);

	pDecl = NULL;
	pDxDevice->CreateVertexDeclaration(vPC, &pDecl);
	pApp->SetPCDecl(pDecl);

	pDecl = NULL;
	pDxDevice->CreateVertexDeclaration(vPs, &pDecl);
	pApp->SetParticleDecl(pDecl);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

//this function is used because the name which is passed to create frame is PCTSTR and the name in the frame is LPCSTR
//and they cannot be casted.
void CopyString(const char* input, char** output) {
	if(input) {
		int length = strlen(input) + 1;
		*output = new char[length];
		strcpy_s(*output, length, input);
	}
	else {
		*output = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////