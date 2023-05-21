#include <stdafx.h>
#include "Misc.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

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