#pragma once
#include"GameEngine.h"
using namespace std;

#define release(x) { if( x ){ (x)->Release();(x) = NULL; } }

/////////////////////////////////////////////////////////////////

void GetWorldPickingRay(D3DXVECTOR3& vOrigin, D3DXVECTOR3& vDirection);

float GetStringWidth(string strString);

float GetStringHeight(string strString);

bool HasNormals(ID3DXMesh* pMesh);

void InitVertexDeclarations();

void CopyString(const char* input, char** output);

/////////////////////////////////////////////////////////////////

const float EPSILON  = 0.001f;
const float INFINITY = FLT_MAX;
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

//bounding box structure TR
struct AABB 
{
	AABB()
		: minPt(INFINITY, INFINITY, INFINITY),
		  maxPt(-INFINITY, -INFINITY, -INFINITY){}

    D3DXVECTOR3 center()const
	{
		return (minPt+maxPt)*0.5f;
	}

	//transforms bounding box according to a matrix
	void transformByMatrix(const D3DXMATRIX& M, AABB& out)
	{
		vector<D3DXVECTOR3> corners;
		corners.resize(8);
		//using the min and max points of the untransformed bounding box we 
		//can calculate the 8 corners of the bounding box
		corners[0] = D3DXVECTOR3( minPt.x, minPt.y, minPt.z ); // min point
		corners[1] = D3DXVECTOR3( maxPt.x, minPt.y, minPt.z ); 
		corners[2] = D3DXVECTOR3( minPt.x, maxPt.y, minPt.z ); 
		corners[3] = D3DXVECTOR3( maxPt.x, maxPt.y, minPt.z ); 
		corners[4] = D3DXVECTOR3( minPt.x, minPt.y, maxPt.z ); 
		corners[5] = D3DXVECTOR3( maxPt.x, minPt.y, maxPt.z );
		corners[6] = D3DXVECTOR3( minPt.x, maxPt.y, maxPt.z );
		corners[7] = D3DXVECTOR3( maxPt.x, maxPt.y, maxPt.z ); // max point

		//transform the 8 corners of the bounding box by the matrix
		for(vector<D3DXVECTOR3>::iterator it = corners.begin(); it != corners.end(); it++)
		{
			D3DXVec3TransformCoord(&(*it),&(*it),&M);
		}


		out.minPt.x = out.maxPt.x=corners[0].x;
		out.minPt.y = out.maxPt.y=corners[0].y;
		out.minPt.z = out.maxPt.z=corners[0].z;

		for(vector<D3DXVECTOR3>::iterator it = corners.begin(); it != corners.end(); it++)
		{
			   if( (*it).x < out.minPt.x ) 
				   out.minPt.x=(*it).x;

			   if( (*it).x > out.maxPt.x ) 
				   out.maxPt.x=(*it).x;

			   if( (*it).y < out.minPt.y ) 
				   out.minPt.y=(*it).y;

			   if( (*it).y > out.maxPt.y ) 
				   out.maxPt.y=(*it).y;

			   if( (*it).z < out.minPt.z ) 
				   out.minPt.z=(*it).z;

			   if( (*it).z > out.maxPt.z ) 
				   out.maxPt.z=(*it).z;
		}

		corners.clear();
	}

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
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




