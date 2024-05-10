#pragma once
#include "log.h"
#include "MyRaylib.h"
#include "Event.h"

class Services;
class SkelAnimation;
class UI;
class Skeleton;
class Segment;
class Joint;


class Editor : public EventListener
{
private:

	// Ptr of global services
	Services* _services;

	std::unique_ptr<SkelAnimation> _skelAnimation;
	std::unique_ptr<UI> _UI;

	std::vector<std::pair<std::string, Texture2D>> _textures;

	// Keys
	bool _keyDeletePressed;
	bool _keyLeftPressed;
	bool _keyUpPressed;
	bool _keyRightPressed;
	bool _keyDownPressed;
	bool _keyCommaPressed;
	bool _keyPeriodPressed;
	bool _keyPlusPressed;
	bool _keyMinusPressed;

	// Mouse keys
	bool _keyLeftMousePressed;
	bool _keyLeftMouseDown;
	bool _keyRightMouseDown;

	// Mouse
	Vector2 _mousePos;
	Vector2 _mouseDelta;
	float _mouseWheel;
	Vector2 _cursorPos;

	// Camera
	Rectangle _editorRec;
	Camera2D _editorCam = {0};

	// Current
	Skeleton* _currentSkeleton = nullptr;
	Segment* _currentSegment = nullptr;
	Joint* _currentJoint = nullptr;
	std::vector<Segment*> _segments;
	std::vector<Joint*> _joints;

	// Flags
	bool _newSegment = false;
	bool _newJoint = false;
	bool _follow;

	bool _play = false;
	unsigned int _playSpeed = 100;
	bool _loop = false;

	bool _showBack = false;
	unsigned int _showBackAmount = 1;
	unsigned int _showBackOpacity = 200;


	void Init();

	void AddSelfAsListener() override;
	void OnEvent(std::shared_ptr<const Event>& event) override;

	void GetInputs();

	void FrameUpdate();
	void Deselect();

	void LoadTex(const std::string& filePath, Segment* segment);

	void SaveData(const std::string& filePath);
	void LoadData(const std::string& filePath);

	void UpdateCamera();
	void UpdateSelection();
	void UpdateEditorInput();

	void UpdateEditor();
	void DrawEditor();
public:

	Editor(Services* servicesIn);
	~Editor();

	void Update();
	void Draw();
};