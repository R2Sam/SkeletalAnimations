#include "UI.h"
#include "Services.h"
#include "EventHandler.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SkelAnimation.h"
#include "Skeleton.h"

UI::UI(Services* servicesIn) : _services(servicesIn)
{
	AddSelfAsListener();
	Init();
}

UI::~UI()
{
	_services->GetEventHandler()->RemoveListener(_ptr);
}

void UI::Init()
{
	LogWindowRectangle logWindowRectangle = {Rectangle{0, 0, 864, 80}, Vector2{648, 680}, GRAY, 10, 5};
	_logWindow.Init(logWindowRectangle, 20, BLACK);
}

void UI::AddSelfAsListener()
{
	// Groups to listen to
	_services->GetEventHandler()->AddListener(_ptr);
	_services->GetEventHandler()->AddLocalListener("UI" ,_ptr);
}

void UI::OnEvent(std::shared_ptr<const Event>& event)
{
	if (std::shared_ptr<const LogWindowMessageEvent> logWindowMessageEvent = std::dynamic_pointer_cast<const LogWindowMessageEvent>(event))
	{
		_logWindow.AddMessage(logWindowMessageEvent->message);
	}
}

void UI::GetInputs()
{
	// Mouse
	_mousePos = GetMousePosition();
	_mouseWheel = GetMouseWheelMove();
}

void UI::Update(SkelAnimation* skelAnimation)
{
	GetInputs();

	if (_newSegmentPressed)
	{
		std::unique_ptr<const Event> event = std::make_unique<const NewSegmentEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_newJointPressed)
	{
		std::unique_ptr<const Event> event = std::make_unique<const NewJointEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_lastFramePressed)
	{
		skelAnimation->LastFrame();

		std::unique_ptr<const Event> event = std::make_unique<const FrameChangedEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_nextFramePressed)
	{
		skelAnimation->NextFrame();

		std::unique_ptr<const Event> event = std::make_unique<const FrameChangedEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	std::pair<unsigned int, unsigned int> pair = skelAnimation->GetCurrentFrame();
	std::string currentFrameString = std::to_string(pair.first) + " / " + std::to_string(pair.second);
	strncpy(_currentFrameText, currentFrameString.c_str(), sizeof(_currentFrameText));

	if (_newFramePressed)
	{
		skelAnimation->NewFrame();

		_logWindow.AddMessage("Created new frame");

		std::unique_ptr<const Event> event = std::make_unique<const FrameChangedEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_deleteFramePressed)
	{
		skelAnimation->DeleteFrame();

		_logWindow.AddMessage("Deleted current frame");

		std::unique_ptr<const Event> event = std::make_unique<const FrameChangedEvent>();
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_playPressed)
	{
		std::unique_ptr<const Event> event = std::make_unique<const PlayEvent>(_loopToggle, _speedAmountValue);
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	std::string lengthString = DoubleToRoundedString(skelAnimation->GetDuration(), 2);
	strncpy(_lengthText, lengthString.c_str(), sizeof(_lengthText));


	skelAnimation->SetFrameTime((float)(1.0 / _animationFPSValue));

	static bool showBack = _showBackToggle;
	static unsigned int amount = _showBackAmountValue;
	static unsigned int opacity = _showBackOpacityValue;
	if (showBack != _showBackToggle || amount != _showBackAmountValue || opacity != _showBackOpacityValue)
	{
		showBack = _showBackToggle;
		amount = _showBackAmountValue;
		opacity = _showBackOpacityValue;

		std::unique_ptr<const Event> event = std::make_unique<const ShowBackEvent>(_showBackToggle, _showBackAmountValue, _showBackOpacityValue);
		_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
	}

	if (_setTexturePressed)
	{
		if (_setTextureText[0] != '\0')
		{
			std::unique_ptr<const Event> event = std::make_unique<const SetTexureEvent>(std::string(_setTextureText));
			_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
		}
	}

	if (_saveDataPressed)
	{
		if (_saveLoadText[0] != '\0')
		{
			std::unique_ptr<const Event> event = std::make_unique<const SaveEvent>(std::string(_saveLoadText));
			_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
		}
	}

	if (_loadPressed)
	{
		if (_saveLoadText[0] != '\0')
		{
			std::unique_ptr<const Event> event = std::make_unique<const LoadEvent>(std::string(_saveLoadText));
			_services->GetEventHandler()->AddLocalEvent("Editor", std::move(event));
		}
	}

	_logWindow.Update(_mousePos, _mouseWheel);
}

void UI::Draw()
{
	DrawRectangle(0, 0, 216, 720, LIGHTGRAY);

	_newSegmentPressed = GuiButton(Rectangle{24, 14, 72, 24}, "Segment");
	_newJointPressed = GuiButton(Rectangle{120, 14, 72, 24}, "Joint");
	_lastFramePressed = GuiButton(Rectangle{24, 62, 72, 24}, "<--");
	_nextFramePressed = GuiButton(Rectangle{120, 62, 72, 24}, "-->");
	GuiStatusBar(Rectangle{48, 110, 120, 24}, _currentFrameText);
	_newFramePressed = GuiButton(Rectangle{24, 158, 72, 24}, "New Frame");
	_deleteFramePressed = GuiButton(Rectangle{120, 158, 72, 24}, "Delete Frame");
	_playPressed = GuiButton(Rectangle{24, 206, 72, 24}, "Play");
	GuiToggle(Rectangle{120, 206, 72, 24}, "Loop", &_loopToggle);
	GuiStatusBar(Rectangle{48, 238, 120, 24}, _lengthText);
	if (GuiSpinner(Rectangle{48, 278, 120, 24}, "%", &_speedAmountValue, 1, 100, _speedEditMode)) _speedEditMode = !_speedEditMode;
	if (GuiSpinner(Rectangle{48, 326, 120, 24}, "FPS", &_animationFPSValue, 1, 60, _speedEditMode)) _speedEditMode = !_speedEditMode;
	GuiToggle(Rectangle{64, 374, 88, 21}, "Show back", &_showBackToggle);
	if (GuiSpinner(Rectangle{48, 406, 120, 24}, "Amount", &_showBackAmountValue, 1, 10, _showBackAmountEditMode)) _showBackAmountEditMode = !_showBackAmountEditMode;
	if (GuiSpinner(Rectangle{48, 446, 120, 24}, "Opacity", &_showBackOpacityValue, 1, 100, _showBackOpacityEditMode)) _showBackOpacityEditMode = !_showBackOpacityEditMode;
	_setTexturePressed = GuiButton(Rectangle{48, 494, 120, 24}, "Set Texture");
	if (GuiTextBox(Rectangle{48, 542, 120, 24}, _setTextureText, 128, _setTextureEditMode)) _setTextureEditMode = !_setTextureEditMode;
	_saveDataPressed = GuiButton(Rectangle{24, 590, 72, 24}, "Save Data");
	_saveAnimationPressed = GuiButton(Rectangle{120, 590, 72, 24}, "Save Anim");
	if (GuiTextBox(Rectangle{48, 638, 120, 24}, _saveLoadText, 128, _saveLoadEditMode)) _saveLoadEditMode = !_saveLoadEditMode;
	_loadPressed = GuiButton(Rectangle{48, 686, 120, 24}, "Load");


	_logWindow.Draw();
}