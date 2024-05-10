#pragma once
#include "log.h"
#include "MyRaylib.h"

#include "Skeleton.h"
#include "Serialization.h"

class Services;
struct AnimationInfo;

class SkelAnimation
{
private:

	Services* _services;

	unsigned int _fameCount = 0;
	float _frameTime = 1;
	float _duration = 0;

	std::vector<Skeleton> _frames;

	Skeleton* _currentSkeleton = nullptr;
	unsigned int _currentFrame = 0;

public:

	SkelAnimation(Services* servicesIn);
	~SkelAnimation();

	void NewFrame();
	void DeleteFrame();

	AnimationInfo GetAnimation() const;
	void LoadAnimation(const AnimationInfo& animation);

	Skeleton* GetCurrentSkeleton();
	std::pair<unsigned int, unsigned int> GetCurrentFrame();
	float GetDuration();

	void SetFrameTime(const float& frameTime);

	void NextFrame();
	void LastFrame();

	void DrawCurrentFrame(const bool& textures, const bool& segments, const bool& joints, const unsigned int& opacity);
	void DrawPasteFrame(const unsigned int& amount, const bool& textures, const bool& segments, const bool& joints, const float& opacity, const float& opacityDropoff);

	bool DrawAnimation(const bool& loop, const unsigned int percentage);
};

struct AnimationInfo
{
	std::vector<Skeleton> frames;
	float frameTime;
};
