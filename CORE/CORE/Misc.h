#pragma once
#include "Application.h"

//needed for GetWorldPickingRay, which should be accessed from everywhere, which has access to camera
class Camera;

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

inline void ReleaseX(IUnknown* resource) {
	if(resource != nullptr) {
		resource->Release();
	}
}

#define CheckSuccess(X) __CheckHR(__FILE__, __LINE__, X)
inline void __CheckHR(std::string file, int line, HRESULT hr) {
	if (FAILED(hr)) {
		std::string errString = DXGetErrorString(hr);
		std::string message = "File:" + file + " line: " + std::to_string(line) + "\nError std::string:" + errString.c_str() + "\nError description:" + DXGetErrorDescription(hr);
		MessageBox(0, message.c_str(), "Error", MB_CANCELTRYCONTINUE | MB_ICONEXCLAMATION);
		if (!errString.compare("D3DXERR_INVALIDDATA")) {
			printf("the filename is either empty or incorrect\n");
		}
		else {
			PostQuitMessage(0);
		}
	}
}

/////////////////////////////////////////////////////////////////

void CopyString(const char* input, char** output);
bool HasNormals(ID3DXMesh* pMesh);

/////////////////////////////////////////////////////////////////

const float EPSILON  = 0.001f;
const float INF = FLT_MAX;
const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR PINK(1.0f, 0.1f, 0.5f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR YELLOW(1.0f, 1.0f, 0.0f, 1.0f);

/////////////////////////////////////////////////////////////////

struct Material {
	D3DXCOLOR m_ambient;
	D3DXCOLOR m_diffuse;
	D3DXCOLOR m_specular;
	float 	  m_fSpecularPower;
};

/////////////////////////////////////////////////////////////////

struct Light {
	D3DXCOLOR m_ambient;
	D3DXCOLOR m_diffuse;
	D3DXCOLOR m_specular;
	D3DXVECTOR3 m_vLight;
};

/////////////////////////////////////////////////////////////////