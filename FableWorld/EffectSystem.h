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
	EffectSystem(string sShaderFileName, string sShaderTechName,string sTextureFileName,int nMaxAmountOfParticles);
	~EffectSystem();

	void		InitShader(string sShaderFileName, string sShaderTechName);

	virtual void AddParticle();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual void OnRender();
	virtual void OnUpdate(float dt);

	virtual void InitParticle(Particle& pParticle) = 0;

protected:

	const int m_nMaxAmountOfParticles;

	float m_fTime;

	vector<Particle> m_vParticles;
	vector<Particle*> m_vDeadParticles;

	ID3DXEffect* m_pEffectShader;
	D3DXHANDLE 	 m_hEffectTechnique;
	D3DXHANDLE 	 m_hWVPMatrix;
	D3DXHANDLE 	 m_hTexture;
	D3DXHANDLE	 m_hTime;
	D3DXHANDLE	 m_hCameraPos;

	IDirect3DTexture9* m_pTexture;

	IDirect3DVertexBuffer9* m_pEffectVertexBuffer;
};