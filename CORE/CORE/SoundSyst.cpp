#include "SoundSyst.h"

/////////////////////////////////////////////////////////////////////////

SoundSyst* soundsyst;

/////////////////////////////////////////////////////////////////////////

SoundSyst::SoundSyst(int nMaxChannels)
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_pSoundSystem);		

	if (result != FMOD_OK)
	{
		MessageBox(0,"Failed creating main object for fmod",0,0);
	}

	result = m_pSoundSystem->init(nMaxChannels, FMOD_INIT_NORMAL, 0);

	if (result != FMOD_OK)
	{
		MessageBox(0,"Failed initializing fmod",0,0);
	}
}

/////////////////////////////////////////////////////////////////////////

//creates sound that is played all the time or after ceratin event in the background
void SoundSyst::CreateBackgroundSound(SoundObject& SoundObj)
{
	m_pSoundSystem->createSound(SoundObj.m_strSoundFileName.c_str(), FMOD_DEFAULT, 0, &SoundObj.m_pSound);
}

/////////////////////////////////////////////////////////////////////////

void SoundSyst::PlayBackgroundSound(SoundObject& SoundObj)
{
	if( !SoundObj.m_bIsPlaying )
	{
		m_pSoundSystem->playSound(FMOD_CHANNEL_FREE, SoundObj.m_pSound, false, &SoundObj.m_pChannel);
		SoundObj.m_pChannel->setVolume(SoundObj.m_fVolume);
		SoundObj.m_pChannel->setLoopCount(1000);
		SoundObj.m_bIsPlaying = true;
	}
}

/////////////////////////////////////////////////////////////////////////

void SoundSyst::Create3DSound(SoundObject& SoundObj)
{
	m_pSoundSystem->createSound(SoundObj.m_strSoundFileName.c_str(), FMOD_3D, 0, &SoundObj.m_pSound);
	SoundObj.m_pSound->set3DMinMaxDistance(SoundObj.m_fMinDistance, SoundObj.m_fMaxDistance);
}

/////////////////////////////////////////////////////////////////////////

void SoundSyst::Play3DSound(SoundObject& SoundObj)
{
	if( !SoundObj.m_bIsPlaying )
	{
		m_pSoundSystem->playSound(FMOD_CHANNEL_FREE, SoundObj.m_pSound, true, &SoundObj.m_pChannel);
		SoundObj.m_pChannel->set3DAttributes(&SoundObj.m_vSoundPosition, 0);
		SoundObj.m_pChannel->setVolume(SoundObj.m_fVolume);
		SoundObj.m_pChannel->setPaused(false);
		SoundObj.m_bIsPlaying = true;
	}
}


/////////////////////////////////////////////////////////////////////////

void SoundSyst::OnUpdate()
{
	//these positions are needed for 3d sounds, so we can determine if we are close or far from a 3d sound, which impacts on the sound
	m_vPos.x = camera->GetPosition().x;
	m_vPos.y = camera->GetPosition().y;
	m_vPos.z = camera->GetPosition().z;

	m_vLook.x = camera->GetLookVector().x;
	m_vLook.y = camera->GetLookVector().y;
	m_vLook.z = camera->GetLookVector().z;

	m_vUp.x = camera->GetUpVector().x;
	m_vUp.y = camera->GetUpVector().y;
	m_vUp.z = camera->GetUpVector().z;

	m_pSoundSystem->set3DListenerAttributes(0,&m_vPos,0,&m_vLook,&m_vUp);
	m_pSoundSystem->update();
}

/////////////////////////////////////////////////////////////////////////

void SoundSyst::AddSoundObject(SoundObject& SoundObj)
{
	m_mapSoundObjects[SoundObj.m_strSoundFileName] = SoundObj;
}

/////////////////////////////////////////////////////////////////////////

void SoundSyst::PlayAllSounds()
{
	for(map<string, SoundObject>::iterator it = m_mapSoundObjects.begin(); it != m_mapSoundObjects.end(); it++)
	{
		if( (*it).second.m_bIs2D )
		{
			PlayBackgroundSound((*it).second);
		}
		else if( (*it).second.m_bIs3D )
		{
			Play3DSound((*it).second);
		}
	}
}

/////////////////////////////////////////////////////////////////////////