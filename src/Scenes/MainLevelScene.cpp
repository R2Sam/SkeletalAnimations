#include "MainLevelScene.h"
#include "Services.h"
#include "EventHandler.h"
#include "SceneHandler.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SkelAnimation.h"

MainLevelScene::MainLevelScene(Services* servicesIn) : _services(servicesIn)
{
	AddSelfAsListener();
	Init();
}

MainLevelScene::~MainLevelScene()
{
	_services->GetEventHandler()->RemoveListener(_ptr);

	for (auto& pair : _textures)
	{
		UnloadTexture(pair.second);
	}
}

void MainLevelScene::Init()
{
	// Editor
	_editorRec = {300, 0, 780, 720};

	_editorCam.zoom = 1;
	_editorCam.offset = {730, 330};

	_skelAnimation = std::make_unique<SkelAnimation>(_services);
	_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
}

void MainLevelScene::AddSelfAsListener()
{
	// Groups to listen to
	_services->GetEventHandler()->AddListener(_ptr);
	_services->GetEventHandler()->AddLocalListener("MainLevel" ,_ptr);
}

void MainLevelScene::OnEvent(std::shared_ptr<const Event>& event)
{
	// If not active ignore events
	if (!_active)
		return;
}

void MainLevelScene::GetInputs()
{
	// Get keys pressed
	_keyEscapePressed = IsKeyPressed(KEY_ESCAPE);
	_keyDeletePressed = IsKeyPressed(KEY_DELETE);
	_keySpacePressed = IsKeyPressed(KEY_SPACE);

	_keyLeftMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	_keyLeftMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	_keyRightMouseDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

	_mousePos = GetMousePosition();
	_mouseDelta = GetMouseDelta();
	_mouseWheel = GetMouseWheelMove();
}

void MainLevelScene::Enter()
{
	_active = true;
}

void MainLevelScene::Exit()
{
	_active = false;
}

void MainLevelScene::UpdateEditor()
{
	_cursorPos = GetScreenToWorld2D(_mousePos, _editorCam);
	UpdateCamera();

	bool selectedSomething = false;

	_joints = _currentSkeleton->GetJoints();
	for (Joint* joint : _joints)
	{
		if (CheckCollisionPointRec(_cursorPos, Rectangle{joint->position.x - 5, joint->position.y - 5, 10, 10}) && _keyLeftMousePressed && !_selectedSegment && !_selectedJoint)
		{
			joint->selected = true;
			_selectedJoint = joint;
			selectedSomething = true;
			break;
		}
	}

	_segments = _currentSkeleton->GetSegments();
	for (Segment* segment : _segments)
	{
		if (CheckCollisionPointRec(_cursorPos, Rectangle{segment->topPos.x - 5, segment->topPos.y - 5, 10, 10}) && _keyLeftMousePressed && !_selectedSegment && !_selectedJoint)
		{
			_topSelected = true;
			_selectedSegment = segment;
			selectedSomething = true;
			break;
		}

		if (CheckCollisionPointRec(_cursorPos, Rectangle{segment->bottomPos.x - 5, segment->bottomPos.y - 5, 10, 10}) && _keyLeftMousePressed && !_selectedSegment && !_selectedJoint)
		{
			_bottomSelected = true;
			_selectedSegment = segment;
			selectedSomething = true;
			break;
		}
	}

	if (_keySpacePressed)
	{
		if (_follow)
		{
			_follow = false;
		}

		else
		{
			_follow = true;
		}
	}

	if (!selectedSomething && _keyLeftMousePressed && !CheckCollisionPointRec(_mousePos, Rectangle{0, 0, 300, 720}) && (_selectedSegment || _selectedJoint))
	{
		_selectedSegment = nullptr;
		_topSelected = false;
		_bottomSelected = false;

		if (_selectedJoint)
		{
			_selectedJoint->selected = false;
		}
		_selectedJoint = nullptr;

		_follow = false;
	}

	if (_selectedSegment && _follow)
	{
		if (_topSelected)
		{
			_selectedSegment->topPos = _cursorPos;
			UpdateSegment(*_selectedSegment);
		}

		else if (_bottomSelected)
		{
			_selectedSegment->bottomPos = _cursorPos;
			UpdateSegment(*_selectedSegment);
		}
	}

	if (_selectedJoint && _follow)
	{
		_selectedJoint->position = _cursorPos;

		for (Segment* segment : _selectedJoint->segments)
		{
			if (segment->jointTop == _selectedJoint)
			{
				segment->topPos = _selectedJoint->position;
				UpdateSegment(*segment);
			}

			if (segment->jointBottom == _selectedJoint)
			{
				segment->bottomPos = _selectedJoint->position;
				UpdateSegment(*segment);
			}
		}
	}

	for (Segment* segment : _segments)
	{
		bool found = false;

		if (!IsTextureReady(segment->texture))
		{

			if (!segment->textureName.empty())
			{
				for (const auto& pair : _textures)
				{
					if (pair.first == segment->textureName)
					{
						segment->texture = pair.second;
						found = true;
						break;
					}
				}

				if (!found)
				{
					if (FileExists(segment->textureName.c_str()))
					{
						_textures.push_back(std::make_pair(segment->textureName, LoadTexture(segment->textureName.c_str())));
						segment->texture = _textures[_textures.size() -1].second;
					}
				}
			}
		}
	}

	UpdateUI();
}

void MainLevelScene::UpdateUI()
{
	if (_newSegmentPressed)
	{
		static unsigned int id = 0;
		id++;
		Segment segment = {0};
		segment.scale = 1;
		segment.ID = id;
		segment.topPos = {100 + 10 * id, 100};
		segment.bottomPos = {100 + 10 * id, 50};

		if (_selectedJoint)
		{
			segment.topPos = _selectedJoint->position;
			segment.bottomPos = {segment.topPos.x, segment.topPos.y - 100};
			segment.jointTop = _selectedJoint;
			segment.jointTopID = _selectedJoint->ID;

			UpdateSegment(segment);
			_selectedJoint->segments.push_back(_currentSkeleton->AddSegment(segment));
			_selectedJoint->segmentIDs.push_back(segment.ID);
		}

		else
		{
			UpdateSegment(segment);
			_currentSkeleton->AddSegment(segment);
		}
		
		_selectedSegment = nullptr;
		_topSelected = false;
		_bottomSelected = false;

		if (_selectedJoint)
		{
			_selectedJoint->selected = false;
		}
		_selectedJoint = nullptr;
	}

	if (_newJointPressed && !_follow && _selectedSegment && !_selectedJoint && (_topSelected || _bottomSelected))
	{
		static unsigned int id = 0;
		id++;
		Joint joint;
		joint.ID = id;

		if (_topSelected && !_selectedSegment->jointTop)
		{
			joint.position = _selectedSegment->topPos;
			joint.segments.push_back(_selectedSegment);
			joint.segmentIDs.push_back(_selectedSegment->ID);

			_selectedSegment->jointTop = _currentSkeleton->AddJoint(joint);
			_selectedSegment->jointTopID = joint.ID;
		}

		else if (_bottomSelected && !_selectedSegment->jointBottom)
		{
			joint.position = _selectedSegment->bottomPos;
			joint.segments.push_back(_selectedSegment);
			joint.segmentIDs.push_back(_selectedSegment->ID);

			_selectedSegment->jointBottom = _currentSkeleton->AddJoint(joint);
			_selectedSegment->jointBottomID = joint.ID;
		}

		_selectedSegment = nullptr;
		_topSelected = false;
		_bottomSelected = false;

		if (_selectedJoint)
		{
			_selectedJoint->selected = false;
		}
		_selectedJoint = nullptr;
	}

	if (_keyDeletePressed && _selectedSegment)
	{
		for (int i = 0; i < _segments.size(); i++)
		{
			if (_segments[i] == _selectedSegment)
			{
				for (Joint* joint : _joints)
				{
					for (int j = 0; j < joint->segments.size(); j++)
					{
						if (joint->segments[j] == _selectedSegment)
						{
							joint->segments.erase(joint->segments.begin() + j);
							joint->segments.shrink_to_fit();

							joint->segmentIDs.erase(joint->segmentIDs.begin() + j);
							joint->segmentIDs.shrink_to_fit();
						}
					}
				}

				_selectedSegment = nullptr;
				_topSelected = false;
				_bottomSelected = false;

				_segments.erase(_segments.begin() + i);
				_segments.shrink_to_fit();

				std::vector<Segment> segmentsObjs;
				for (Segment* segment : _segments)
				{
					segmentsObjs.push_back(*segment);
				}

				_currentSkeleton->SetSegments(segmentsObjs);
				break;
			}
		}
	}

	if (_keyDeletePressed && _selectedJoint)
	{
		for (int i = 0; i < _joints.size(); i++)
		{
			if (_joints[i] == _selectedJoint)
			{
				for (Segment* segment : _segments)
				{
					if (segment->jointTop == _selectedJoint)
					{
						segment->jointTop = nullptr;
						segment->jointTopID = 0;

						break;
					}

					if (segment->jointBottom == _selectedJoint)
					{
						segment->jointBottom = nullptr;
						segment->jointBottomID = 0;

						break;
					}
				}

				_selectedJoint = nullptr;

				_joints.erase(_joints.begin() + i);
				_joints.shrink_to_fit();

				std::vector<Joint> jointsObjs;
				for (Joint* joint : _joints)
				{
					jointsObjs.push_back(*joint);
				}

				_currentSkeleton->SetJoints(jointsObjs);
				break;
			}
		}
	}

	static float fpsValue = _fpsValue;
	if (_fpsValue != fpsValue)
	{
		_skelAnimation->SetFrameTime(1 / _fpsValue);
		fpsValue = _fpsValue;
	}

	if (_playPressed)
	{
		_playing = true;
	}

	if (_nextFramePressed)
	{
		_skelAnimation->NextFrame();
		_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
		_joints = _currentSkeleton->GetJoints();
		_segments = _currentSkeleton->GetSegments();
	}

	if (_lastFramePressed)
	{
		_skelAnimation->LastFrame();
		_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
		_joints = _currentSkeleton->GetJoints();
		_segments = _currentSkeleton->GetSegments();
	}

	if (_newFramePressed)
	{
		_skelAnimation->NewFrame();
		_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
		_joints = _currentSkeleton->GetJoints();
		_segments = _currentSkeleton->GetSegments();
	}

	if (_deleteFramePressed)
	{
		_skelAnimation->DeleteFrame();
		_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
		_joints = _currentSkeleton->GetJoints();
		_segments = _currentSkeleton->GetSegments();
	}

	if (_setTexturePressed && _selectedSegment)
	{
		if (FileExists(_setTexturePathText))
		{
			_textures.push_back(std::make_pair(std::string(_setTexturePathText), LoadTexture(_setTexturePathText)));

			_selectedSegment->texture = _textures[_textures.size() -1].second;
			_selectedSegment->textureName = std::string(_setTexturePathText);
		}

		else
		{
			Log("File not found");
		}
	}

	static float angle = _textureAngleValue;
	if (angle != _textureAngleValue && _selectedSegment)
	{
		_selectedSegment->rotation = _textureAngleValue;
		angle = _textureAngleValue;
	}
}

void MainLevelScene::UpdateCamera()
{
	if (CheckCollisionPointRec(_mousePos, _editorRec) && _keyRightMouseDown)
	{
		Vector2 delta = _mouseDelta;
        delta = Vector2Scale(delta, -1.0f/_editorCam.zoom);

        _editorCam.target = Vector2Add(_editorCam.target, delta);

        static const Vector2 maxTarget = {650, 610};
        static const Vector2 minTarget = {-570, -670};

        _editorCam.target = Vector2Clamp(_editorCam.target, minTarget, maxTarget);

        _keyRightMouseDown = false;
	}

	if (_mouseWheel != 0)
	{
		float zoomAmount = _editorCam.zoom;

		if (_mouseWheel > 0)
		{
			zoomAmount *= 1.5;
			zoomAmount = std::min(zoomAmount, (float)25);
		}

		if (_mouseWheel < 0)
		{

			zoomAmount /= 1.5;
			zoomAmount = std::max(zoomAmount, (float)0.75);
		}

		_editorCam.zoom = zoomAmount;
	}
}

void MainLevelScene::DrawEditor()
{
	DrawRectangleRec(_editorRec, WHITE);

	BeginMode2D(_editorCam);

	// Grid
	int spacing = 10;
	int max = 1000;
	int reps = (max / spacing) * 2;
	for (int i = 0; i < reps + 1; i++)
	{
		DrawLine(-max, -max + spacing * i, max, -max + spacing * i, Color{0, 0, 0, 150});
		DrawLine(-max + spacing * i, max, -max + spacing * i, -max, Color{0, 0, 0, 150});
	}

	if (_topSelected && _selectedSegment)
	{
		DrawRectangleRec(Rectangle{_selectedSegment->topPos.x - 5, _selectedSegment->topPos.y - 5, 10, 10}, RED);
	}

	if (_bottomSelected && _selectedSegment)
	{
		DrawRectangleRec(Rectangle{_selectedSegment->bottomPos.x - 5, _selectedSegment->bottomPos.y - 5, 10, 10}, RED);
	}


	if (!_playing)
	{
		if (_backShowActive)
		{
			_skelAnimation->DrawPasteFrame(_backShowAmountValue, true, true, true, 200, 225 / _backShowAmountValue);
			_skelAnimation->DrawCurrentFrame(true, true, true, 255);
		}

		else
		{
			_skelAnimation->DrawCurrentFrame(true, true, true, 225);
		}
	}

	else
	{
		_playing = !_skelAnimation->DrawAnimation(_loopActive, _speedValue);
	}

	// Center
	DrawRectangle(-2.5, -2.5, 5, 5, BLACK);

    EndMode2D();

    std::stringstream cursorPosText;
    cursorPosText << std::fixed << std::setprecision(2) << "(X: " << _cursorPos.x << " Y: " << _cursorPos.y << ")";
	DrawText(cursorPosText.str().c_str(), 450, 675, 20, BLACK);
	cursorPosText.str("");
}

void MainLevelScene::DrawUI()
{
	DrawRectangle(0, 0, 300, 720, WHITE);

	_newSegmentPressed = GuiButton((Rectangle){ 48, 48, 120, 24 }, "New Segment"); 
    _newJointPressed = GuiButton((Rectangle){ 48, 96, 120, 24 }, "New Joint"); 
    if (GuiSpinner((Rectangle){ 48, 624, 120, 24 }, "Angle", &_textureAngleValue, 0, 359, _textureAngleEditMode)) _textureAngleEditMode = !_textureAngleEditMode;
    if (GuiSpinner((Rectangle){ 48, 144, 120, 24 }, "FPS", &_fpsValue, 1, 60, _fpsEditMode)) _fpsEditMode = !_fpsEditMode;
    if (GuiSpinner((Rectangle){ 48, 216, 120, 24 }, "Amout", &_backShowAmountValue, 1, 10, _backShowAmountEditMode)) _backShowAmountEditMode = !_backShowAmountEditMode;
    GuiToggle((Rectangle){ 64, 184, 88, 24 }, "Back Show", &_backShowActive);
    _lastFramePressed = GuiButton((Rectangle){ 48, 264, 48, 24 }, "<---"); 
    _nextFramePressed = GuiButton((Rectangle){ 120, 264, 48, 24 }, "--->"); 
    _playPressed = GuiButton((Rectangle){ 48, 312, 120, 24 }, "PLAY");
    if (_playing)
    {
    	DrawTriangle(Vector2{188, 312}, Vector2{188, 312 + 24}, Vector2{188 + 20, 312 + 12}, RED);
    }
    if (GuiSpinner((Rectangle){ 48, 384, 120, 24 }, "Speed", &_speedValue, 1, 100, _speedEditMode)) _speedEditMode = !_speedEditMode;
    GuiToggle((Rectangle){ 64, 344, 88, 24 }, "Loop", &_loopActive);
    _newFramePressed = GuiButton((Rectangle){ 48, 432, 120, 24 }, "New Frame"); 
    _deleteFramePressed = GuiButton((Rectangle){ 48, 480, 120, 24 }, "Delete Frame"); 
    if (GuiTextBox((Rectangle){ 48, 528, 120, 24 }, _setTexturePathText, 128, _setTexturePathEditMode)) _setTexturePathEditMode = !_setTexturePathEditMode;
    _setTexturePressed = GuiButton((Rectangle){ 48, 576, 120, 24 }, "Set Texture"); 
}

void MainLevelScene::Update()
{
	if(!_active)
		return;

	GetInputs();

	// Exit scene and go to menu
	if (_keyEscapePressed && _active)
	{
		std::unique_ptr<const Event> event = std::make_unique<const NextSceneEvent>("Menu");
		_services->GetEventHandler()->AddLocalEvent("SceneHandler", std::move(event));

		_keyEscapePressed = false;
		_active = false;
		return;
	}

	UpdateEditor();
}

void MainLevelScene::Draw()
{
	if(!_active)
		return;

	DrawEditor();

	DrawUI();
}