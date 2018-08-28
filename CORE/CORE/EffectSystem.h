#include"Misc.h"
#include"Camera.h"

//////////////////////////////////////////////////////////////////////////////////////////

struct Particle
{
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_velocity;
	float       m_size;
	float		m_initialTime;
	float		m_lifeTime;
	float		m_mass;
	D3DXCOLOR   m_color;
};

//////////////////////////////////////////////////////////////////////////////////////////

class EffectSystem
{
public:
	EffectSystem(std::string sShaderFileName, std::string sShaderTechName, std::string sTextureFileName, int nMaxAmountOfParticles, D3DXVECTOR4 accel, float timePerParticle);
	virtual ~EffectSystem();

	void		InitShader(std::string sShaderFileName, std::string sShaderTechName);

	virtual void AddParticle();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual void OnRender();
	virtual void OnUpdate(float dt);

	virtual void InitParticle(Particle& pParticle) = 0;

	void SetTime(float time);

protected:

	int m_particlesAmount;

	float m_time;
	int m_timePerParticle;
	D3DXVECTOR4 m_accel;

	std::vector<Particle> m_particles;

	std::vector<Particle*> m_aliveParticles;
	std::vector<Particle*> m_deadParticles;

	ID3DXEffect* m_pEffectShader;
	D3DXHANDLE 	 m_hEffectTechnique;
	D3DXHANDLE 	 m_hWVPMatrix;
	D3DXHANDLE 	 m_hTexture;
	D3DXHANDLE	 m_hTime;
	D3DXHANDLE	 m_hCameraPos;
	D3DXHANDLE	 m_hAcceleration;

	IDirect3DTexture9* m_pTexture;

	IDirect3DVertexBuffer9* m_pEffectVertexBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////////