#include "AnimationComponent.h"

AnimationComponent::AnimationComponent()
{
	m_bShouldPlayAnimationOnce = false;
	m_bShouldStopTrackAfterPlayingAnimation = false;
}

ID3DXAnimationController* AnimationComponent::GetAnimationController()
{
	return m_pAnimationController;
}

bool AnimationComponent::ShouldPlayAnimationOnce()
{
	return m_bShouldPlayAnimationOnce;
}

void AnimationComponent::SetShouldPlayAnimationOnce(bool shouldPlayAnimationOnce)
{
	m_bShouldPlayAnimationOnce = shouldPlayAnimationOnce;
}

bool AnimationComponent::ShouldStopTrackAfterPlayingAnimation()
{
	return m_bShouldStopTrackAfterPlayingAnimation;
}

void AnimationComponent::SetShouldStopTrackAfterPlayingAnimation(bool shouldStopTrackAfterPlayingAnimation)
{
	m_bShouldStopTrackAfterPlayingAnimation = shouldStopTrackAfterPlayingAnimation;
}

void AnimationComponent::EnableTrack(DWORD track, bool bEnable)
{
	m_pAnimationController->SetTrackEnable(track, bEnable);
}

void AnimationComponent::AdvanceTime(float dt)
{
	m_pAnimationController->AdvanceTime(dt, nullptr);
}

void AnimationComponent::SetAnimationOnTrack(PCSTR animationName, AnimationTrack track)
{
	m_pAnimationController->GetAnimationSetByName(animationName, &m_pCurrentAnimationSet);
	m_pAnimationController->SetTrackAnimationSet(track, m_pCurrentAnimationSet);
}

double AnimationComponent::GetTrackPosition(AnimationTrack track)
{
	D3DXTRACK_DESC trackDescription;
	m_pAnimationController->GetTrackDesc(track, &trackDescription);

	return trackDescription.Position;
}

double AnimationComponent::GetTrackAnimationSetDuration(AnimationTrack track)
{
	m_pAnimationController->GetTrackAnimationSet(track, &m_pCurrentAnimationSet);
	return m_pCurrentAnimationSet->GetPeriod();
}

void AnimationComponent::SetTrackSpeed(AnimationTrack track, float newValue, float startTime, float transitionPeriod)
{
	m_pAnimationController->KeyTrackSpeed(track, newValue, startTime, transitionPeriod, D3DXTRANSITION_LINEAR);
}

void AnimationComponent::SetTrackWeight(AnimationTrack track, float newValue, float startTime, float transitionPeriod)
{
	m_pAnimationController->KeyTrackWeight(track, newValue, startTime, transitionPeriod, D3DXTRANSITION_LINEAR);
}

double AnimationComponent::GetGlobalTime()
{
	return m_pAnimationController->GetTime();
}

void AnimationComponent::SetTrackPosition(AnimationTrack track, float position)
{
	m_pAnimationController->SetTrackPosition(track, position);
}
