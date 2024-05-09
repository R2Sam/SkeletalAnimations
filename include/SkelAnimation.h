#pragma once
#include "log.h"
#include "MyRaylib.h"

#include "Skeleton.h"
#include "Serialization.h"

class Services;

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

	Skeleton* GetCurrentSkeleton();

	void SetFrameTime(const float& frameTime);

	void NextFrame();
	void LastFrame();

	void DrawCurrentFrame(const bool& textures, const bool& segments, const bool& joints, const unsigned int& opacity);
	void DrawPasteFrame(const unsigned int& amount, const bool& textures, const bool& segments, const bool& joints, const unsigned int& opacity, const unsigned int& opacityDropoff);

	bool DrawAnimation(const bool& loop, const unsigned int percentage);
};