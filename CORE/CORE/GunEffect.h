#include "EffectSystem.h"
#include "SkinnedModel.h"

class GunEffect: public EffectSystem
{
public:

	GunEffect(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName, int nMaxAmountOfParticles, D3DXVECTOR4 accel, int timePerParticle);
	virtual ~GunEffect();

	virtual void OnUpdate(float dt) override;

private:
	virtual void InitParticle(Particle& pParticle) override;

	SkinnedModel* m_pGalio;
	SkinnedModel* m_pCho;
};