#include "SkelAnimation.h"
#include "Services.h"

SkelAnimation::SkelAnimation(Services* servicesIn) : _services(servicesIn)
{
	Skeleton skeleton;
	_frames.push_back(skeleton);
	_currentSkeleton = &_frames[0];

	_fameCount++;
	_duration = _fameCount * _frameTime;

	_currentFrame = 1;
}

SkelAnimation::~SkelAnimation()
{

}

void SkelAnimation::NewFrame()
{
	Skeleton skeleton (&(_frames[(_currentFrame - 1)]));
	
	_frames.insert(_frames.begin() + (_currentFrame), skeleton);
	_fameCount++;
	_currentFrame++;
	_duration = _fameCount * _frameTime;

	_currentSkeleton = &_frames[(_currentFrame - 1)];

	Log("Created New Frame: " << _currentFrame);
}

void SkelAnimation::DeleteFrame()
{
	_frames.erase(_frames.begin() + (_currentFrame - 1));
	_fameCount--;
	_currentFrame--;

	if (_frames.size() == 0)
	{
		Skeleton skeleton;
		_frames.push_back(skeleton);
		_currentSkeleton = &_frames[0];

		_fameCount++;
		_duration = _fameCount * _frameTime;

		_currentFrame = 1;
	}

	else
	{
		_currentSkeleton = &_frames[(_currentFrame - 1)];
		_duration = _fameCount * _frameTime;
	}

	Log("Deleted Frame");
}

AnimationInfo SkelAnimation::GetAnimation() const
{
	AnimationInfo animation;

	animation.frames = _frames;
	animation.frameTime = _frameTime;

	return animation;
}

void SkelAnimation::LoadAnimation(const AnimationInfo& animation)
{
	_frames = animation.frames;
	_frameTime = animation.frameTime;
	_fameCount = _frames.size();
	_duration = _fameCount * _frameTime;

	_currentFrame = 1;
	_currentSkeleton = &_frames[_currentFrame - 1];
}

Skeleton* SkelAnimation::GetCurrentSkeleton()
{
	return _currentSkeleton;
}

std::pair<unsigned int, unsigned int> SkelAnimation::GetCurrentFrame()
{
	return std::make_pair(_currentFrame, _fameCount);
}

float SkelAnimation::GetDuration()
{
	return _duration;
}

void SkelAnimation::SetFrameTime(const float& frameTime)
{
	_frameTime = frameTime;
	_duration = _frameTime * _fameCount;
}

void SkelAnimation::NextFrame()
{
	if(_currentFrame < _fameCount)
	{
		_currentFrame++;
		_currentSkeleton = &_frames[(_currentFrame - 1)];
	}

	Log("Next Frame: " << _currentFrame);
}

void SkelAnimation::LastFrame()
{
	if(_currentFrame > 1)
	{
		_currentFrame--;
		_currentSkeleton = &_frames[(_currentFrame - 1)];
	}

	Log("Last Frame: " << _currentFrame);
}

void SkelAnimation::DrawCurrentFrame(const bool& textures, const bool& segments, const bool& joints, const unsigned int& opacity)
{
	if (textures)
	{
		_currentSkeleton->DrawTextures(opacity);
	}

	if (segments)
	{
		_currentSkeleton->DrawSegments(opacity);
	}

	if (joints)
	{
		_currentSkeleton->DrawJoints(opacity);
	}
}

void SkelAnimation::DrawPasteFrame(const unsigned int& amount, const bool& textures, const bool& segments, const bool& joints, const unsigned int& opacity, const unsigned int& opacityDropoff)
{
	int currentOpacity = opacity + opacityDropoff;

	for (int i = amount; i > 0; i--)
	{
		if ((_currentFrame - i) > 0)
		{
			currentOpacity -= opacityDropoff;
			currentOpacity = std::max(currentOpacity, 0);

			Skeleton* skeleton = &_frames[(_currentFrame - i) - 1];
			if (textures)
			{
				skeleton->DrawTextures(currentOpacity);
			}

			if (segments)
			{
				skeleton->DrawSegments(currentOpacity);
			}

			if (joints)
			{
				skeleton->DrawJoints(currentOpacity);
			}
		}

		else
		{
			break;
		}

	}
}

bool SkelAnimation::DrawAnimation(const bool& loop, const unsigned int percentage)
{
	static unsigned int currentFrame = 1;
	static float time = 0;
	time += _services->deltaT;

	if (time >= (_frameTime * (float)(1 / (float)(percentage / 100))))
	{
		time = 0;

		if (currentFrame != _fameCount)
		{
			currentFrame++;
		}
	}

	_frames[(currentFrame - 1)].DrawTextures(255);

	if (loop && currentFrame ==  _fameCount)
	{
		currentFrame = 1;
	}

	if (!loop && currentFrame ==  _fameCount)
	{
		return true;
	}

	return false;
}