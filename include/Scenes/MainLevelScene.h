#pragma once
#include "log.h"
#include "MyRaylib.h"
#include "Scene.h"
#include "Event.h"

class Services;

class SkelAnimation;

#include "Skeleton.h"

class MainLevelScene : public Scene, public EventListener
{
private:

	// Ptr of global services
	Services* _services;

	// Keys
	bool _keyEscapePressed;
	bool _keyDeletePressed;
	bool _keySpacePressed;
	bool _keyLeftMousePressed;
	bool _keyLeftMouseDown;
	bool _keyRightMouseDown;

	// Mouse
	Vector2 _mousePos;
	Vector2 _mouseDelta;
	float _mouseWheel;
	Vector2 _cursorPos;

	// Editor
	Rectangle _editorRec;
	Camera2D _editorCam = {0};
	std::unique_ptr<SkelAnimation> _skelAnimation;
	Skeleton* _currentSkeleton;
	std::vector<Segment*> _segments;
	std::vector<Joint*> _joints;
	Segment* _selectedSegment = nullptr;
	bool _topSelected = false;
	bool _bottomSelected = false;
	Joint* _selectedJoint = nullptr;
	bool _follow = false;
	bool _playing = false;
	std::vector<std::pair<std::string, Texture2D>> _textures;

	// UI
	bool _newSegmentPressed = false;
    bool _newJointPressed = false;
    bool _textureAngleEditMode = false;
    int _textureAngleValue = 0;
    bool _fpsEditMode = false;
    int _fpsValue = 1;
    bool _backShowAmountEditMode = false;
    int _backShowAmountValue = 1;
    bool _backShowActive = false;
    bool _lastFramePressed = false;
    bool _nextFramePressed = false;
    bool _playPressed = false;
    bool _speedEditMode = false;
    int _speedValue = 100;
    bool _loopActive = false;
    bool _newFramePressed = false;
    bool _deleteFramePressed = false;
    bool _setTexturePathEditMode = false;
    char _setTexturePathText[128] = "";
    bool _setTexturePressed = false;



	void Init() override;

	void AddSelfAsListener() override;
	void OnEvent(std::shared_ptr<const Event>& event) override;

	void GetInputs() override;

	void UpdateEditor();
	void UpdateUI();
	void UpdateCamera();

	void DrawEditor();
	void DrawUI();

public:

	MainLevelScene(Services* servicesIn);
	~MainLevelScene();

	void Enter() override;
	void Exit() override;

	void Update() override;
	void Draw() override;
};