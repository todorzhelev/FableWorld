#include"EffectSystem.h"
#include"Camera.h"

EffectSystem::EffectSystem(string sShaderFileName, string sShaderTechName, string sTextureFileName,int nMaxAmountOfParticles)
:m_nMaxAmountOfParticles(nMaxAmountOfParticles),m_fTime(0.f)
{
	D3DXCreateTextureFromFile(pDxDevice,sTextureFileName.c_str(),&m_pTexture);


	InitShader(sShaderFileName,sShaderTechName);


	pDxDevice->CreateVertexBuffer(nMaxAmountOfParticles*sizeof(Particle),
								  D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,
								  0, D3DPOOL_DEFAULT, &m_pEffectVertexBuffer, 0);

}

EffectSystem::~EffectSystem()
{
	//for(auto it = m_vParticles.begin(); it != m_vParticles.end(); it++)
	//{
	//	delete (*it);
	//}
}

void EffectSystem::AddParticle()
{
	Particle* pNewParticle = new Particle;

	InitParticle(*pNewParticle);

	m_vParticles.push_back(*pNewParticle);

	
}

void EffectSystem::InitShader(string sShaderFileName, string sShaderTechName)
{
	if(FAILED(D3DXCreateEffectFromFile(pDxDevice,sShaderFileName.c_str(),0,0,D3DXSHADER_DEBUG,0,&m_pEffectShader,0)))
	{
		MessageBox(0,"Failed loading effect file in EffectSystem",0,0);
		PostQuitMessage(0);
	}

	m_hEffectTechnique	  = m_pEffectShader->GetTechniqueByName(sShaderTechName.c_str());
	m_hWVPMatrix		  = m_pEffectShader->GetParameterByName(0, "WVP");
	m_hTexture	 		  = m_pEffectShader->GetParameterByName(0, "text");
	m_hTime				  = m_pEffectShader->GetParameterByName(0, "EffectTime");
	m_hCameraPos		  = m_pEffectShader->GetParameterByName(0, "CameraPos");

}

void EffectSystem::OnLostDevice()
{
	m_pEffectShader->OnLostDevice();

	m_pEffectVertexBuffer->Release();
}

void EffectSystem::OnResetDevice()
{
	m_pEffectShader->OnResetDevice();

	pDxDevice->CreateVertexBuffer(m_nMaxAmountOfParticles*sizeof(Particle),
								  D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,
								  0, D3DPOOL_DEFAULT, &m_pEffectVertexBuffer, 0);
}

void EffectSystem::OnUpdate(float dt)
{
	

}

void EffectSystem::OnRender()
{
	m_pEffectShader->SetTechnique(m_hEffectTechnique);

	D3DXVECTOR4 vCameraPos = camera->GetPosition();

	/*m_pEffectShader->SetVector(m_hCameraPos,&vCameraPos);
	m_pEffectShader->SetMatrix(m_hWVPMatrix, &(camera->GetViewProjMatrix()));*/


	m_pEffectShader->SetVector(m_hCameraPos,&vCameraPos);
	m_pEffectShader->SetMatrix(m_hWVPMatrix, &(camera->GetViewProjMatrix()));

	m_pEffectShader->SetFloat(m_hTime,m_fTime);
	m_pEffectShader->SetTexture(m_hTexture,m_pTexture);

	pDxDevice->SetStreamSource(0, m_pEffectVertexBuffer, 0, sizeof(Particle));

	pDxDevice->SetVertexDeclaration(pEngine->GetParticleDeclaration());

	UINT numPasses = 0;
	m_pEffectShader->Begin(&numPasses, 0);
	//m_pEffectShader->BeginPass(0);

	for(UINT i =0;i<numPasses;++i)
	{
		m_pEffectShader->BeginPass(i);
				
			Particle* p = 0;
			m_pEffectVertexBuffer->Lock(0, 0,  (void**)&p, 0);

			int nCount = 0;



			for( int i = 0; i < m_vParticles.size(); i++ )
			{
				p[nCount] = m_vParticles[i];
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