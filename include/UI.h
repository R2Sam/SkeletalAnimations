#pragma once
#include "log.h"
#include "MyRaylib.h"
#include "Event.h"

#include "LogWindow.h"

class Services;
class SkelAnimation;

class UI : public EventListener
{
private:

	// Ptr of global services
	Services* _services;

	// Log
	LogWindow _logWindow;

	// Mouse
	Vector2 _mousePos;
	float _mouseWheel;

	// UI elements returns
	bool _newSegmentPressed = false;
	bool _newJointPressed = false;
	bool _lastFramePressed = false;
	bool _nextFramePressed = false;
	char _currentFrameText[128] = "";
	bool _newFramePressed = false;
	bool _deleteFramePressed = false;
	bool _playPressed = false;
	bool _loopToggle = false;
	char _lengthText[128] = "";
	int _speedAmountValue = 100;
	int _animationFPSValue = 1;
	bool _showBackToggle = false;
	int _showBackAmountValue = 1;
	int _showBackOpacityValue = 100;
	bool _setTexturePressed = false;
	char _setTextureText[128] = "";
	bool _saveDataPressed = false;
	bool _saveAnimationPressed = false;
	char _saveLoadText[128] = "";
	bool _loadPressed = false;


	// UI elements internal flags
	bool _speedEditMode = false;
	bool _animationFPSEditMode = false;
	bool _showBackAmountEditMode = false;
	bool _showBackOpacityEditMode = false;
	bool _setTextureEditMode = false;
	bool _saveLoadEditMode = false;

	void Init();

	void AddSelfAsListener() override;
	void OnEvent(std::shared_ptr<const Event>& event) override;

	void GetInputs();

public:

	UI(Services* servicesIn);
	~UI();

	void Update(SkelAnimation* skelAnimation);
	void Draw();
};

class NewSegmentEvent : public Event
{

};

class NewJointEvent : public Event
{

};

class FrameChangedEvent : public Event
{

};

class PlayEvent : public Event
{
public:

	bool loop;
	float speed;

	PlayEvent(const bool& loopIn, const unsigned int& speedIn) : loop(loopIn), speed(speedIn) {}
};

class ShowBackEvent : public Event
{
public:

	unsigned int amount;
	unsigned int opacity;

	ShowBackEvent(const unsigned int& amountIn, const unsigned int& opacityIn) : amount(amountIn), opacity(opacityIn) {}
};

class SetTexureEvent : public Event
{
public:

	std::string texturePath;

	SetTexureEvent(const std::string& texturePathIn) : texturePath(texturePathIn) {}
};

class SaveEvent : public Event
{
public:

	std::string filePath;

	SaveEvent(const std::string& filePathIn) : filePath(filePathIn) {}
};

class LoadEvent : public Event
{
public:

	std::string filePath;

	LoadEvent(const std::string& filePathIn) : filePath(filePathIn) {}
};

class LogWindowMessageEvent : public Event
{
public:

	std::string message;

	LogWindowMessageEvent(const std::string& messageIn) : message(messageIn) {}
};