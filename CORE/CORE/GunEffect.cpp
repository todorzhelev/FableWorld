#include <stdafx.h>
#include "GunEffect.h"

//////////////////////////////////////////////////////////////////////////////////////
//TODO:refactor everything

GunEffect::GunEffect(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName,int nMaxAmountOfParticles)
	:EffectSystem(sShaderFileName,sShaderTechName,sTextureFileName,nMaxAmountOfParticles)
{
	for (auto& gameObject : m_pGameObjManager->GetGameObjects())
	{
		if (!gameObject->GetName().compare("galio"))
		{
			m_pGalio = static_cast<SkinnedModel*>(gameObject);
			//break;
		}

		if (!gameObject->GetName().compare("cho"))
		{
			m_pCho = static_cast<SkinnedModel*>(gameObject);
			//break;
		}
	}
	  
}

//////////////////////////////////////////////////////////////////////////////////////

GunEffect::~GunEffect()
{
}

//////////////////////////////////////////////////////////////////////////////////////

void GunEffect::InitParticle(Particle& pParticle)
{
	pParticle.m_vPos = D3DXVECTOR3(-20, 100, -6);
	//pParticle.m_vPos = camera->GetPosition();

	pParticle.m_vPos.y += 3000;

	D3DXVECTOR3 diff = m_pGalio->GetPosition() - m_pCho->GetPosition();

	//D3DXVECTOR3 diff = pEzreal->m_vRight;

	//D3DXVec3Normalize(&diff,&diff);

	//pParticle->m_vPos.y -= 3.0f;

	float speed = 500.0f;

	D3DXVECTOR3 vec = m_pGalio->GetLookVector();

	pParticle.m_vVelocity = speed * vec;

	//pParticle.m_vVelocity = speed*diff;

	pParticle.m_fInitialTime = m_fTime;
	pParticle.m_fLifetime = 4.0f;
	pParticle.m_Color = PINK;
	pParticle.m_fSize = 100.0;
	pParticle.m_fMass = 1.0f;
}

//////////////////////////////////////////////////////////////////////////////////////

void GunEffect::OnUpdate(float dt)
{
	m_fTime +=dt;

	//D3DXVECTOR3 accel(0,-9.8f,0);
	D3DXVECTOR3 accel(0,1,0);
	D3DXVec3Normalize(&accel,&accel);

	for(int i=0; i < m_vParticles.size(); i++ )
	{
		float t = m_fTime - m_vParticles[i].m_fInitialTime;
		//float t = 1;

		//m_vParticles[i].m_vPos = m_vParticles[i].m_vPos + m_vParticles[i].m_vVelocity*t + 0.5f *accel* t * t;

		//ddfsd
		m_vParticles[i].m_vPos = m_vParticles[i].m_vPos + m_vParticles[i].m_vVelocity*t;

		Particle part = m_vParticles.back();

		//m_pGalio->SetPosition(part.m_vPos);
	}
	

	//float3 acceleration = float3(0,-9.8,0);

	//inp.pos = (inp.pos + inp.velocity*t + 0.5f * t * t);
}

//////////////////////////////////////////////////////////////////////////////////////

