#include "EffectSystem.h"


class GunEffect: public EffectSystem
{
public:

	 GunEffect(string sShaderFileName, string sShaderTechName,string sTextureFileName,int nMaxAmountOfParticles);
	~GunEffect();

	virtual void OnUpdate(float dt);

private:
	virtual void InitParticle(Particle& pParticle);
};