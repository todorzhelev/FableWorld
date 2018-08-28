#include"Misc.h"
#include"Camera.h"

struct Particle
{
	float m_fInitialTime;
	float m_fMass;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vVelocity;
	float m_fLifetime;

	float       m_fSize;
	D3DXCOLOR    m_Color;
};


class EffectSystem
{

public:
	EffectSystem(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName,int nMaxAmountOfParticles);
	virtual ~EffectSystem();

	void		InitShader(std::string sShaderFileName, std::string sShaderTechName);

	virtual void AddParticle();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual void OnRender();
	virtual void OnUpdate(float dt);

	virtual void InitParticle(Particle& pParticle) = 0;

protected:

	const int m_nMaxAmountOfParticles;

	float m_fTime;

	std::vector<Particle> m_vParticles;
	std::vector<Particle*> m_vDeadParticles;

	ID3DXEffect* m_pEffectShader;
	D3DXHANDLE 	 m_hEffectTechnique;
	D3DXHANDLE 	 m_hWVPMatrix;
	D3DXHANDLE 	 m_hTexture;
	D3DXHANDLE	 m_hTime;
	D3DXHANDLE	 m_hCameraPos;

	IDirect3DTexture9* m_pTexture;

	IDirect3DVertexBuffer9* m_pEffectVertexBuffer;
};