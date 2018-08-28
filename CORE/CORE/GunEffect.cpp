#include <stdafx.h>
#include "GunEffect.h"

//////////////////////////////////////////////////////////////////////////////////////

GunEffect::GunEffect(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName,int nMaxAmountOfParticles, D3DXVECTOR4 accel, int timePerParticle)
	:EffectSystem(sShaderFileName,sShaderTechName,sTextureFileName,nMaxAmountOfParticles,accel,timePerParticle)
{
	  
}

//////////////////////////////////////////////////////////////////////////////////////

GunEffect::~GunEffect()
{
}

//////////////////////////////////////////////////////////////////////////////////////

void GunEffect::InitParticle(Particle& outParticle)
{
	//TODO: make it more generic
	auto initPos = m_pGameObjManager->GetObjectByName("galio")->GetPosition();

	outParticle.m_pos = initPos;

	// Set down a bit so it looks like player is carrying
	// the gun.
	outParticle.m_pos.y += 15.0f;

	// Fire in camera's look direction.
	float speed = 500.0f;
	outParticle.m_velocity = speed * camera->GetLookVector();

	outParticle.m_initialTime = m_time;
	outParticle.m_lifeTime = 4.0f;
	outParticle.m_color = WHITE;
	outParticle.m_size = 100.f;
	outParticle.m_mass = 1.0f;
}

//////////////////////////////////////////////////////////////////////////////////////

void GunEffect::OnUpdate(float dt)
{
	EffectSystem::OnUpdate(dt);

}

//////////////////////////////////////////////////////////////////////////////////////

