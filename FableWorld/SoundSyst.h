#pragma once
#include"Camera.h"
#include "Misc.h"
#include<map>

//////////////////////////////////////////////////////////////////////////////

class SoundSyst
{
public:
	SoundSyst(int nMaxChannels);

	void CreateBackgroundSound(SoundObject& SoundObj);

	void PlayBackgroundSound(SoundObject& SoundObj);

	void Create3DSound(SoundObject& SoundObj);

	void Play3DSound(SoundObject& SoundObj);

	void OnUpdate();

	void AddSoundObject(SoundObject& SoundObj);

	void PlayAllSounds();

private:

	map<string, SoundObject> m_mapSoundObjects;

	FMOD::System* m_pSoundSystem;

	FMOD_VECTOR m_vPos;
	FMOD_VECTOR m_vUp;
	FMOD_VECTOR m_vLook;
};

//////////////////////////////////////////////////////////////////////////////

extern SoundSyst* soundsyst;

//////////////////////////////////////////////////////////////////////////////