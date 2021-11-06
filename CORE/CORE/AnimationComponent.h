#pragma once
#include "Misc.h"

enum AnimationTrack {
	FirstTrack,
	SecondTrack
};

class AnimationComponent {
public:
	AnimationComponent();
	ID3DXAnimationController* GetAnimationController();
	bool ShouldPlayAnimationOnce();
	void SetShouldPlayAnimationOnce(bool shouldPlayAnimationOnce);
	bool ShouldStopTrackAfterPlayingAnimation();
	void SetShouldStopTrackAfterPlayingAnimation(bool shouldStopTrackAfterPlayingAnimation);
	void EnableTrack(DWORD track, bool bEnable);
	void AdvanceTime(float dt);
	void SetAnimationOnTrack(PCSTR animationName, AnimationTrack track);
	double GetTrackPosition(AnimationTrack track);
	double GetTrackAnimationSetDuration(AnimationTrack track);
	void SetTrackSpeed(AnimationTrack track, float newValue, float startTime, float transitionPeriod = 0.125f);
	void SetTrackWeight(AnimationTrack track, float newValue, float startTime, float transitionPeriod = 0.125f);
	void OnUpdate(float dt);
	void PlayAnimation(LPCSTR strAnimationName);
	void PlayAnimationOnceAndStopTrack(LPCSTR strAnimationName);
	void PlayAnimationOnce(LPCSTR strAnimationName);
	bool JustStartedPlayingAnimationOnce();
	double GetGlobalTime();
	void SetTrackPosition(AnimationTrack track, float position);
	void SetAnimationSpeed(int animationSpeed);
	int GetAnimationSpeed();

	ID3DXAnimationController* m_pAnimationController;

private:
	ID3DXAnimationSet*  m_pCurrentAnimationSet;
	bool  m_bShouldPlayAnimationOnce;
	bool  m_bShouldStopTrackAfterPlayingAnimation;
	//should refactor this
	bool  m_bJustStartedPlayingAnimationOnce;
	int m_animationSpeed;
};