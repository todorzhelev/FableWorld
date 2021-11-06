#include <stdafx.h>
#include "AnimationComponent.h"

AnimationComponent::AnimationComponent()
{
	m_bShouldPlayAnimationOnce = false;
	m_bShouldStopTrackAfterPlayingAnimation = false;
	m_animationSpeed = 1;
}

/////////////////////////////////////////////////////////////////////////////

ID3DXAnimationController* AnimationComponent::GetAnimationController()
{
	return m_pAnimationController;
}

/////////////////////////////////////////////////////////////////////////////

bool AnimationComponent::ShouldPlayAnimationOnce()
{
	return m_bShouldPlayAnimationOnce;
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetShouldPlayAnimationOnce(bool shouldPlayAnimationOnce)
{
	m_bShouldPlayAnimationOnce = shouldPlayAnimationOnce;
}

/////////////////////////////////////////////////////////////////////////////

bool AnimationComponent::ShouldStopTrackAfterPlayingAnimation()
{
	return m_bShouldStopTrackAfterPlayingAnimation;
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetShouldStopTrackAfterPlayingAnimation(bool shouldStopTrackAfterPlayingAnimation)
{
	m_bShouldStopTrackAfterPlayingAnimation = shouldStopTrackAfterPlayingAnimation;
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::EnableTrack(DWORD track, bool bEnable)
{
	m_pAnimationController->SetTrackEnable(track, bEnable);
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::AdvanceTime(float dt)
{
	m_pAnimationController->AdvanceTime(dt, nullptr);
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetAnimationOnTrack(PCSTR animationName, AnimationTrack track)
{
	m_pAnimationController->GetAnimationSetByName(animationName, &m_pCurrentAnimationSet);
	m_pAnimationController->SetTrackAnimationSet(track, m_pCurrentAnimationSet);
}

/////////////////////////////////////////////////////////////////////////////

double AnimationComponent::GetTrackPosition(AnimationTrack track)
{
	D3DXTRACK_DESC trackDescription;
	m_pAnimationController->GetTrackDesc(track, &trackDescription);

	return trackDescription.Position;
}

/////////////////////////////////////////////////////////////////////////////

double AnimationComponent::GetTrackAnimationSetDuration(AnimationTrack track)
{
	m_pAnimationController->GetTrackAnimationSet(track, &m_pCurrentAnimationSet);
	return m_pCurrentAnimationSet->GetPeriod();
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetTrackSpeed(AnimationTrack track, float newValue, float startTime, float transitionPeriod)
{
	m_pAnimationController->KeyTrackSpeed(track, newValue, startTime, transitionPeriod, D3DXTRANSITION_LINEAR);
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetTrackWeight(AnimationTrack track, float newValue, float startTime, float transitionPeriod)
{
	m_pAnimationController->KeyTrackWeight(track, newValue, startTime, transitionPeriod, D3DXTRANSITION_LINEAR);
}

/////////////////////////////////////////////////////////////////////////////

double AnimationComponent::GetGlobalTime()
{
	return m_pAnimationController->GetTime();
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetTrackPosition(AnimationTrack track, float position)
{
	m_pAnimationController->SetTrackPosition(track, position);
}


/////////////////////////////////////////////////////////////////////////////

//sets the animation index on the mesh at the current track. 
//This function repeats the animation, i.e. after the animation set ended it starts from the beginning and so on.
void AnimationComponent::PlayAnimation(LPCSTR strAnimationName)
{
	if (!m_bShouldStopTrackAfterPlayingAnimation && !m_bShouldPlayAnimationOnce)
	{
		SetAnimationOnTrack(strAnimationName, FirstTrack);
	}
}

/////////////////////////////////////////////////////////////////////////////

//plays animation only once then stops( currently used for dead animation )
//we use that the second track is occupied by attack animations, so we switch to the first track for
//these kind of animations
void AnimationComponent::PlayAnimationOnceAndStopTrack(LPCSTR strAnimationName)
{
	if (!m_bShouldStopTrackAfterPlayingAnimation)
	{
		SetTrackPosition(FirstTrack, 0.0);
		SetAnimationOnTrack(strAnimationName, FirstTrack);

		SetTrackSpeed(FirstTrack, 1.0f, GetGlobalTime());
		SetTrackWeight(FirstTrack, 1.0f, GetGlobalTime());

		SetTrackSpeed(SecondTrack, 0.0f, GetGlobalTime());
		SetTrackWeight(SecondTrack, 0.0f, GetGlobalTime());

		m_bShouldPlayAnimationOnce = false; //this fixes sudden track resets
		m_bShouldStopTrackAfterPlayingAnimation = true;
	}
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::PlayAnimationOnce(LPCSTR strAnimationName)
{
	//if we are currently playing animation once dont enter here
	if (!m_bShouldPlayAnimationOnce)
	{
		SetTrackPosition(SecondTrack, 0.0);
		SetAnimationOnTrack(strAnimationName, SecondTrack);

		SetTrackSpeed(FirstTrack, 0.0f, GetGlobalTime());
		SetTrackWeight(FirstTrack, 0.0f, GetGlobalTime());

		SetTrackSpeed(SecondTrack, 1.0f, GetGlobalTime());
		SetTrackWeight(SecondTrack, 1.0f, GetGlobalTime());

		m_bShouldPlayAnimationOnce = true;
		m_bJustStartedPlayingAnimationOnce = true;
	}
	else
	{
		m_bJustStartedPlayingAnimationOnce = false;
	}
}

/////////////////////////////////////////////////////////////////////////////

//this is used when attacking. If we just started playing animation once (attacking animation)
//we know that we should lower the health of the enemy just once for the duration of the attack animation
bool AnimationComponent::JustStartedPlayingAnimationOnce()
{
	return m_bJustStartedPlayingAnimationOnce;
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::OnUpdate(float dt)
{
	//after we know that the current animation should be played just once
	//we see when it is finished and we transit to the first track, which holds the idle animation set
	if (m_bShouldPlayAnimationOnce)
	{
		auto trackPosition = GetTrackPosition(SecondTrack);
		auto duration = GetTrackAnimationSetDuration(SecondTrack);

		//after the attack animation has finished we slightly make transition to idle animation.
		if (abs(trackPosition - duration) < 0.01f)
		{
			SetTrackSpeed(FirstTrack, 1.0f, GetGlobalTime());
			SetTrackWeight(FirstTrack, 1.0f, GetGlobalTime());

			SetTrackSpeed(SecondTrack, 0.0f, GetGlobalTime());
			SetTrackWeight(SecondTrack, 0.0f, GetGlobalTime());

			SetTrackPosition(SecondTrack, 0.0);

			m_bShouldPlayAnimationOnce = false;
		}
	}

	//after we play the animation once we have to stop the track
	if (m_bShouldStopTrackAfterPlayingAnimation)
	{
		auto trackPosition = GetTrackPosition(FirstTrack);
		auto duration = GetTrackAnimationSetDuration(FirstTrack);
		//bigger tolerance is needed because the track position tends to get out of the limits, which resets the track to the initial position
		//should think of better solution, since now we rely on a number
		if (abs(trackPosition - duration) < duration*0.25f)
		{
			SetTrackSpeed(FirstTrack, 0.0f, GetGlobalTime());
			//setting the weight to 0.1 prevents the object from disappearing
			SetTrackWeight(FirstTrack, 0.1f, GetGlobalTime());

			SetTrackSpeed(SecondTrack, 0.0f, GetGlobalTime());
			SetTrackWeight(SecondTrack, 0.0f, GetGlobalTime());

			SetTrackPosition(SecondTrack, 0.0); 

			//this prevents from setting another animation while we are closing the track
			//m_bShouldStopTrackAfterPlayingAnimation = false;
		}
	}

	AdvanceTime(dt*m_animationSpeed);
}

/////////////////////////////////////////////////////////////////////////////

void AnimationComponent::SetAnimationSpeed(int animationSpeed)
{
	m_animationSpeed = animationSpeed;
}

/////////////////////////////////////////////////////////////////////////////

int AnimationComponent::GetAnimationSpeed()
{
	return m_animationSpeed;
}

/////////////////////////////////////////////////////////////////////////////