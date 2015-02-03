#include "GunEffect.h"
#include "SkinnedMesh.h"
#include "StaticMesh.h"


GunEffect::GunEffect(string sShaderFileName, string sShaderTechName, string sTextureFileName,int nMaxAmountOfParticles)
	:EffectSystem(sShaderFileName,sShaderTechName,sTextureFileName,nMaxAmountOfParticles)
{

	  
}

void GunEffect::OnUpdate(float dt)
{
	m_fTime +=dt;

	//D3DXVECTOR3 accel(0,-9.8f,0);
	D3DXVECTOR3 accel(0,1,0);
	D3DXVec3Normalize(&accel,&accel);

	//SkinnedMesh* pEzreal = NULL;
	StaticMesh* pEzreal = NULL;
	for(map<string,GameObject*>::iterator it = pEngine->m_mapGameObjects.begin();it!=pEngine->m_mapGameObjects.end();it++)
	{
		if( !(*it).first.compare("tree2") )
		{
			pEzreal = static_cast<StaticMesh*>((*it).second);
			//break;
		}
	}


	for(int i=0; i < m_vParticles.size(); i++ )
	{
		float t = m_fTime - m_vParticles[i].m_fInitialTime;
		//float t = 1;

		//m_vParticles[i].m_vPos = m_vParticles[i].m_vPos + m_vParticles[i].m_vVelocity*t + 0.5f *accel* t * t;

		//ddfsd
		m_vParticles[i].m_vPos = m_vParticles[i].m_vPos + m_vParticles[i].m_vVelocity*t;

		Particle part = m_vParticles.back();

		pEzreal->m_vPos = part.m_vPos;
	}
	

	//float3 acceleration = float3(0,-9.8,0);

	//inp.pos = (inp.pos + inp.velocity*t + 0.5f * t * t);


}


void GunEffect::InitParticle(Particle& pParticle)
{
	SkinnedMesh* pEzreal = NULL;
	SkinnedMesh* pCho = NULL;
	for(map<string,GameObject*>::iterator it = pEngine->m_mapGameObjects.begin();it!=pEngine->m_mapGameObjects.end();it++)
	{
		if( !(*it).first.compare("galio") )
		{
			pEzreal = static_cast<SkinnedMesh*>((*it).second);
			//break;
		}

		if( !(*it).first.compare("cho") )
		{
			pCho = static_cast<SkinnedMesh*>((*it).second);
			//break;
		}
	}

	pParticle.m_vPos = pEzreal->m_vPos;
	//pParticle.m_vPos = camera->GetPosition();

	pParticle.m_vPos.y += 15;

	D3DXVECTOR3 diff = pEzreal->m_vPos - pCho->m_vPos;

	//D3DXVECTOR3 diff = pEzreal->m_vRight;

	//D3DXVec3Normalize(&diff,&diff);

	//pParticle->m_vPos.y -= 3.0f;

	float speed = 500.0f;

	D3DXVECTOR3 vec = pEzreal->m_vLook;

	pParticle.m_vVelocity = speed* vec;

	//pParticle.m_vVelocity = speed*diff;

	pParticle.m_fInitialTime = m_fTime;
	pParticle.m_fLifetime    = 4.0f;
	pParticle.m_Color        = WHITE;
	pParticle.m_fSize        = 10.0;
	pParticle.m_fMass        = 1.0f;
}