#pragma once
#include"Application.h"
using namespace std;

inline void ReleaseX(IUnknown* resource)
{
	if(resource != nullptr)
	{
		resource->Release();
	}
}

#define CheckHR(X) __CheckHR(__FILE__, __LINE__, X)
inline void __CheckHR(std::string file, int line, HRESULT hr)
{
	if (FAILED(hr))
	{
		std::string message = "File:" + file + " line: " + std::to_string(line) + "\nError string:" + DXGetErrorString(hr) + "\nError description:" + DXGetErrorDescription(hr);
		MessageBox(0, message.c_str(), "Error", MB_CANCELTRYCONTINUE | MB_ICONEXCLAMATION);
		exit(1);
	}
}

/////////////////////////////////////////////////////////////////

void GetWorldPickingRay(D3DXVECTOR3& vOrigin, D3DXVECTOR3& vDirection);

float GetStringWidth(string strString);

float GetStringHeight(string strString);

bool HasNormals(ID3DXMesh* pMesh);

void InitVertexDeclarations();

void CopyString(const char* input, char** output);

/////////////////////////////////////////////////////////////////

const float EPSILON  = 0.001f;
const float INF = FLT_MAX;
const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);

/////////////////////////////////////////////////////////////////

struct Material
{
	D3DXCOLOR m_ambient;
	D3DXCOLOR m_diffuse;
	D3DXCOLOR m_specular;
	float 	  m_fSpecularPower;
};

/////////////////////////////////////////////////////////////////

struct Light
{
	D3DXCOLOR m_ambient;
	D3DXCOLOR m_diffuse;
	D3DXCOLOR m_specular;
	D3DXVECTOR3 m_vLight;
};

/////////////////////////////////////////////////////////////////

//struct for sounds(2d and 3d)
struct SoundObject
{
	string			m_strSoundFileName;
	FMOD::Sound*	m_pSound;
	FMOD::Channel*	m_pChannel;
	float			m_fVolume;
	FMOD_VECTOR		m_vSoundPosition;
	float			m_fMinDistance;
	float			m_fMaxDistance;
	bool			m_bIsPlaying;
	bool			m_bIs2D;
	bool			m_bIs3D;
};

/////////////////////////////////////////////////////////////////

//struct for quests
struct QuestObject
{
	string	title;
	string	content;
	bool	completed;
	string	requiredObject;
};

/////////////////////////////////////////////////////////////////

struct VertexPosition
{
	D3DXVECTOR3 m_vPos;
};

/////////////////////////////////////////////////////////////////

struct VertexPositionColor
{
	D3DXVECTOR3 m_vPos;
	D3DXCOLOR   m_color;
};

/////////////////////////////////////////////////////////////////

struct VertexPositionNormal
{
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vNormal;
};

/////////////////////////////////////////////////////////////////

struct VertexPositionTexture
{
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR2 m_vTexCoords;
};

/////////////////////////////////////////////////////////////////

struct VertexPositionNormalTexture
{
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vNormal;
	D3DXVECTOR2 m_vTexCoords;
};

/////////////////////////////////////////////////////////////////




