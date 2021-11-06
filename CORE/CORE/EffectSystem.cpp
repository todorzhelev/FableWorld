#include <stdafx.h>
#include "EffectSystem.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

EffectSystem::EffectSystem(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName,int nMaxAmountOfParticles, D3DXVECTOR4 accel)
:m_particlesAmount(nMaxAmountOfParticles),m_time(0.f), m_accel(accel)
{
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice,sTextureFileName.c_str(),&m_pTexture));


	InitShader(sShaderFileName,sShaderTechName);


	CheckSuccess(pDxDevice->CreateVertexBuffer(nMaxAmountOfParticles*sizeof (Particle),
								  D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,
								  0, D3DPOOL_DEFAULT, &m_pEffectVertexBuffer, 0));


	m_particles.resize(m_particlesAmount);
	m_aliveParticles.reserve(m_particlesAmount);
	m_deadParticles.reserve(m_particlesAmount);

	// They start off all dead.
	for (int i = 0; i < m_particlesAmount; ++i)
	{
		m_particles[i].m_lifeTime = -1.0f;
		m_particles[i].m_initialTime = 0.0f;
	}


}

///////////////////////////////////////////////////////////////////////////////////////////////////////

EffectSystem::~EffectSystem()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::AddParticle(GameObject* object)
{
	if (!object)
	{
		return;
	}

	if (m_deadParticles.size() > 0)
	{
		// Reinitialize a particle.
		Particle* p = m_deadParticles.back();
		InitParticle(*p, object);

		// No longer dead.
		m_deadParticles.pop_back();
		m_aliveParticles.push_back(p);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::InitShader(std::string sShaderFileName, std::string sShaderTechName)
{
	CheckSuccess(D3DXCreateEffectFromFile(pDxDevice, sShaderFileName.c_str(), 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffectShader, 0));

	m_hEffectTechnique	  = m_pEffectShader->GetTechniqueByName(sShaderTechName.c_str());
	m_hWVPMatrix		  = m_pEffectShader->GetParameterByName(0, "WVP");
	m_hCameraPos		  = m_pEffectShader->GetParameterByName(0, "cameraPos");
	m_hTexture	 		  = m_pEffectShader->GetParameterByName(0, "pTexture");
	m_hTime				  = m_pEffectShader->GetParameterByName(0, "pTime");
	m_hAcceleration		  = m_pEffectShader->GetParameterByName(0, "pAccel");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::OnLostDevice()
{
	m_pEffectShader->OnLostDevice();

	m_pEffectVertexBuffer->Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::OnResetDevice()
{
	m_pEffectShader->OnResetDevice();

	CheckSuccess(pDxDevice->CreateVertexBuffer(m_particlesAmount *sizeof(Particle),
								  D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,
								  0, D3DPOOL_DEFAULT, &m_pEffectVertexBuffer, 0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::OnUpdate(float dt)
{
	m_time += dt;

	m_deadParticles.resize(0);
	m_aliveParticles.resize(0);

	for (int i = 0; i < m_particlesAmount; ++i)
	{
		if ((m_time - m_particles[i].m_initialTime) > m_particles[i].m_lifeTime)
		{
			m_deadParticles.push_back(&m_particles[i]);
		}
		else
		{
			m_aliveParticles.push_back(&m_particles[i]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::SetTime(float time)
{
	m_time = time;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void EffectSystem::OnRender()
{
	m_pEffectShader->SetTechnique(m_hEffectTechnique);

	D3DXVECTOR3 camPos = camera->GetPosition();
	D3DXVECTOR4 vCameraPos(camPos, 0);

	m_pEffectShader->SetVector(m_hCameraPos,&vCameraPos);
	D3DXMATRIX viewProjMatrix = camera->GetViewProjMatrix();
	m_pEffectShader->SetMatrix(m_hWVPMatrix, &viewProjMatrix);
	m_pEffectShader->SetFloat(m_hTime,m_time);
	m_pEffectShader->SetTexture(m_hTexture,m_pTexture);
	m_pEffectShader->SetVector(m_hAcceleration, &m_accel);

	pDxDevice->SetStreamSource(0, m_pEffectVertexBuffer, 0, sizeof(Particle));

	CheckSuccess(pDxDevice->SetVertexDeclaration(pApp->GetParticleDecl()));

	UINT numPasses = 0;
	m_pEffectShader->Begin(&numPasses, 0);

	for(UINT i =0;i<numPasses;++i)
	{
		m_pEffectShader->BeginPass(i);
				
			Particle* p = 0;
			m_pEffectVertexBuffer->Lock(0, 0,  (void**)&p, 0);

			int nCount = 0;

			for (int i = 0; i < m_aliveParticles.size(); ++i)
			{
				p[nCount] = *m_aliveParticles[i];
				nCount++;
			}

			m_pEffectVertexBuffer->Unlock();

			if( nCount > 0 )
			{
				pDxDevice->DrawPrimitive(D3DPT_POINTLIST,0,nCount);
			}

		m_pEffectShader->EndPass();
	}
	m_pEffectShader->End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////