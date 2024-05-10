#include "Editor.h"
#include "Services.h"
#include "EventHandler.h"

#include <fstream>

#include "SkelAnimation.h"
#include "UI.h"
#include "Serialization.h"
#include "Skeleton.h"

Editor::Editor(Services* servicesIn) : _services(servicesIn)
{
	AddSelfAsListener();
	Init();
}

Editor::~Editor()
{
	_services->GetEventHandler()->RemoveListener(_ptr);
}

void Editor::Init()
{
	_skelAnimation = std::make_unique<SkelAnimation>(_services);
	_UI = std::make_unique<UI>(_services);

	FrameUpdate();

	// Cam
	_editorRec = {216, 0, 864, 620};
	_editorCam.zoom = 1;
	_editorCam.offset = {648, 340};
}

void Editor::AddSelfAsListener()
{
	// Groups to listen to
	_services->GetEventHandler()->AddListener(_ptr);
	_services->GetEventHandler()->AddLocalListener("Editor" ,_ptr);
	_services->GetEventHandler()->AddLocalListener("UI" ,_ptr);
}

void Editor::OnEvent(std::shared_ptr<const Event>& event)
{
	if (std::shared_ptr<const NewSegmentEvent> newSegmentEvent = std::dynamic_pointer_cast<const NewSegmentEvent>(event))
	{
		_newSegment = true;
	}

	if (std::shared_ptr<const NewJointEvent> newJointEvent = std::dynamic_pointer_cast<const NewJointEvent>(event))
	{
		_newJoint = true;
	}

	if (std::shared_ptr<const FrameChangedEvent> frameChangedEvent = std::dynamic_pointer_cast<const FrameChangedEvent>(event))
	{
		FrameUpdate();

		for (Segment* segment : _segments)
		{
			if (!segment->textureName.empty())
			{
				LoadTex(segment->textureName, segment);
			}

			UpdateSegment(*segment);
		}

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Current Frame: " + std::to_string(_skelAnimation->GetCurrentFrame().first));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (std::shared_ptr<const PlayEvent> playEvent = std::dynamic_pointer_cast<const PlayEvent>(event))
	{
		_play = true;
		_playSpeed = playEvent->speed;
		_loop = playEvent->loop;
	}

	if (std::shared_ptr<const ShowBackEvent> showBackEvent = std::dynamic_pointer_cast<const ShowBackEvent>(event))
	{
		_showBack = showBackEvent->toggle;
		_showBackAmount = showBackEvent->amount;
		_showBackOpacity = showBackEvent->opacity;
	}

	if (std::shared_ptr<const SetTexureEvent> setTexureEvent = std::dynamic_pointer_cast<const SetTexureEvent>(event))
	{
		if (_currentSegment)
		{
			LoadTex(setTexureEvent->texturePath, _currentSegment);
		}

		else
		{
			std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Could not set texture no segment selected");
			_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
		}
	}

	if (std::shared_ptr<const SaveEvent> saveEvent = std::dynamic_pointer_cast<const SaveEvent>(event))
	{
		SaveData(saveEvent->filePath);
	}

	if (std::shared_ptr<const LoadEvent> loadEvent = std::dynamic_pointer_cast<const LoadEvent>(event))
	{
		LoadData(loadEvent->filePath);
	}
}

void Editor::GetInputs()
{
	// Keys
	_keyDeletePressed = IsKeyPressed(KEY_DELETE);
	_keyLeftPressed = IsKeyPressed(KEY_LEFT);
	_keyUpPressed = IsKeyPressed(KEY_UP);
	_keyRightPressed = IsKeyPressed(KEY_RIGHT);
	_keyDownPressed = IsKeyPressed(KEY_DOWN);
	_keyCommaPressed = IsKeyPressed(KEY_COMMA);
	_keyPeriodPressed = IsKeyPressed(KEY_PERIOD);
	_keyPlusPressed = IsKeyPressed(KEY_KP_ADD);
	_keyMinusPressed = IsKeyPressed(KEY_KP_SUBTRACT);

	// Mouse Keys
	_keyLeftMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	_keyLeftMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	_keyRightMouseDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

	_mousePos = GetMousePosition();
	_mouseDelta = GetMouseDelta();
	_mouseWheel = GetMouseWheelMove();
	_cursorPos = GetScreenToWorld2D(_mousePos, _editorCam);
}

void Editor::FrameUpdate()
{
	Deselect();

	_currentSkeleton = _skelAnimation->GetCurrentSkeleton();
	_segments = _currentSkeleton->GetSegments();
	_joints = _currentSkeleton->GetJoints();
}

void Editor::Deselect()
{
	if (_currentSegment)
	{
		_currentSegment->selected = false;
		_currentSegment->selectedTop = false;
		_currentSegment->selectedBottom = false;

		_currentSegment = nullptr;
	}

	if (_currentJoint)
	{
		_currentJoint->selected = false;
		_currentJoint = nullptr;
	}
}

void Editor::LoadTex(const std::string& filePath, Segment* segment)
{
	for (const auto& pair : _textures)
	{
		if (pair.first == filePath)
		{
			segment->texture = pair.second;
			segment->textureName = pair.first;

			std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Set texture: " + pair.first);
			_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));

			return;
		}
	}

	if (FileExists(filePath.c_str()))
	{
		segment->texture = LoadTexture(filePath.c_str());
		segment->textureName = filePath;
		_textures.push_back(std::make_pair(filePath, segment->texture));

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Loaded and set texture: " + filePath);
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	else
	{
		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Could not find texture: " + filePath + "!");
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}
}

void Editor::SaveData(const std::string& filePath)
{
	for (const char& c : filePath)
	{
		if (c == '.')
		{
			std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Plase insert a filename witout a filetype");
			_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));

			return;
		}
	}

	std::string fullFilename = filePath + ".anim";

	if (FileExists(fullFilename.c_str()))
    {
        std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Overriding file: "+ fullFilename);
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
    }

    else
    {
    	std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Creating file: " + fullFilename);
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
    }

    std::ofstream file(fullFilename, std::ios::binary);

    if (file.is_open())
    {
    	cereal::BinaryOutputArchive archive(file);

    	AnimationInfo animation = _skelAnimation->GetAnimation();
    	AnimationData data = SerializeAnimation(&animation);

    	archive(data);

    	file.close();
    }

    else
    {
    	std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Error!! Failed in opening file: " + fullFilename);
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
    }
}

void Editor::LoadData(const std::string& filePath)
{
	for (int i = 0; i < filePath.size(); i++)
	{
		if (filePath[i] == '.' && filePath.size() - i - 1 >= 4)
		{
			if (!(filePath[i + 1] == 'a' && filePath[i + 2] == 'n' && filePath[i + 3] == 'i' && filePath[i + 4] == 'm'))
			{
				std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("File path is not .anim!");
				_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));

				return;
			}
		}
	}

	if (!FileExists(filePath.c_str()))
	{
		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("File cannot be found!");
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));

		return;
	}

	std::ifstream file(filePath, std::ios::binary);

	if (file.is_open())
	{
		AnimationData data;
		cereal::BinaryInputArchive archive(file);
		archive(data);

		_skelAnimation->LoadAnimation(DeSerializeAnimation(data));
		FrameUpdate();

		for (Segment* segment : _segments)
		{
			if (!segment->textureName.empty())
			{
				LoadTex(segment->textureName, segment);
			}

			UpdateSegment(*segment);
		}
	}

	else
	{
		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Error!! Failed in opening file: " + filePath);
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}
}

void Editor::UpdateCamera()
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

	if (CheckCollisionPointRec(_mousePos, _editorRec) && _mouseWheel != 0)
	{
		float zoomAmount = _editorCam.zoom;

		if (_mouseWheel > 0)
		{
			if (zoomAmount >= 1)
			{
				zoomAmount += 0.25;
			}

			else
			{
				zoomAmount += 0.1;
			}
			
			zoomAmount = std::min(zoomAmount, (float)2.5);
		}

		if (_mouseWheel < 0)
		{
			if (zoomAmount > 1)
			{
				zoomAmount -= 0.25;
			}

			else
			{
				zoomAmount -= 0.1;
			}
			
			zoomAmount = std::max(zoomAmount, (float)0.70);
		}

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Zoom: " + DoubleToRoundedString(zoomAmount, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));

		_editorCam.zoom = zoomAmount;
	}
}

void Editor::UpdateSelection()
{	
	// Deselect
	if (_keyLeftMousePressed && (_currentSegment || _currentJoint) && CheckCollisionPointRec(_mousePos, _editorRec))
	{
		Deselect();
	}

	// Check joints
	for (Joint* joint : _joints)
	{
		if (_keyLeftMousePressed && CheckCollisionPointRec(_cursorPos, CenteredRectangle(joint->rec, joint->position)) && !_currentJoint && !_currentSegment)
		{
			joint->selected = true;
			_currentJoint = joint;
			break;
		}
	}

	// Check segments
	for (Segment* segment : _segments)
	{
		// Check top
		if (_keyLeftMousePressed && CheckCollisionPointRec(_cursorPos, CenteredRectangle(segment->recTop, segment->topPos)) && !_currentSegment && !_currentJoint)
		{
			segment->selectedTop = true;
			_currentSegment = segment;
			break;
		}

		// Cheack bottom
		if (_keyLeftMousePressed && CheckCollisionPointRec(_cursorPos, CenteredRectangle(segment->recBot, segment->bottomPos)) && !_currentSegment && !_currentJoint)
		{
			segment->selectedBottom = true;
			_currentSegment = segment;
			break;
		}

		// Check core
		if (_keyLeftMousePressed &&  (CheckCollisionPointTriangle(_cursorPos, segment->topCorners.first, segment->topCorners.second, segment->bottomCorners.first) || CheckCollisionPointTriangle(_cursorPos, segment->bottomCorners.first, segment->bottomCorners.second, segment->topCorners.second)) && !_currentSegment && !_currentJoint)
		{
			segment->selected = true;
			_currentSegment = segment;
			break;
		}
	}
}

void Editor::UpdateEditorInput()
{
	// Deletion of a segment
	if (_keyDeletePressed && _currentSegment)
	{
		for (int i = 0; i <  _segments.size(); i++)
		{
			if (_segments[i] == _currentSegment)
			{
				_segments.erase(_segments.begin() + i);
				_segments.shrink_to_fit();

				std::vector<Segment> segmentsObjs;
				for (Segment* segment : _segments)
				{
					segmentsObjs.push_back(*segment);
				}

				_currentSkeleton->SetSegments(segmentsObjs);

				FrameUpdate();
				break;
			}
		}
	}

	// Deletion of a joint
	if (_keyDeletePressed && _currentJoint)
	{
		for (int i = 0; i < _joints.size(); i++)
		{
			if (_joints[i]->ID == _currentJoint->ID)
			{
				for (Segment* segment : _segments)
				{
					if (segment->jointTopID && segment->jointTopID == _currentJoint->ID)
					{
						segment->jointTopID = 0;
					}

					if (segment->jointBottomID && segment->jointBottomID == _currentJoint->ID)
					{
						segment->jointBottomID = 0;
					}
				}

				_joints.erase(_joints.begin() + i);
				_joints.shrink_to_fit();

				std::vector<Joint> jointsObjs;
				for (Joint* joint : _joints)
				{
					jointsObjs.push_back(*joint);
				}

				_currentSkeleton->SetJoints(jointsObjs);

				FrameUpdate();
				break;
			}
		}
	}

	// New segment
	if (_newSegment && !_currentSegment)
	{
		Segment segment = {0};
		segment.scale = 1;
		segment.thinkness = 2.5;
		segment.topPos = {100, 100};
		segment.bottomPos = {0, 0};

		if (_currentJoint)
		{
			segment.topPos = _currentJoint->position;
			segment.bottomPos = {segment.topPos.x + 25, segment.topPos.y + 25};
			segment.jointTopID = _currentJoint->ID;

			UpdateSegment(segment);
			_currentSkeleton->AddSegment(segment);
		}

		else
		{
			UpdateSegment(segment);
			_currentSkeleton->AddSegment(segment);
		}

		FrameUpdate();
		_newSegment = false;
	}

	// New joint
	if (_newJoint)
	{
		if (_currentSegment && (_currentSegment->selectedTop || _currentSegment->selectedBottom))
		{
			if (_currentSegment->selectedTop && !_currentSegment->jointTopID)
			{
				Joint joint;
				joint.position = _currentSegment->topPos;

				Joint* ptr = _currentSkeleton->AddJoint(joint);
				_currentSegment->jointTopID = ptr->ID;
			}

			else if (_currentSegment->selectedBottom && !_currentSegment->jointBottomID)
			{
				Joint joint;
				joint.position = _currentSegment->bottomPos;

				Joint* ptr = _currentSkeleton->AddJoint(joint);
				_currentSegment->jointBottomID = ptr->ID;
			}

			else
			{
				std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Could not create joint, segment end already has one!");
				_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
			}
		}

		else
		{
			std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Could not create joint, no segment end selected!");
			_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
		}

		FrameUpdate();
		_newJoint = false;
	}

	if (_keyLeftPressed && _currentSegment)
	{
		_currentSegment->offset.x -= 1;

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture x offset: " + DoubleToRoundedString(_currentSegment->offset.x, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyUpPressed && _currentSegment)
	{
		_currentSegment->offset.y -= 1;

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture y offset: " + DoubleToRoundedString(-_currentSegment->offset.y, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyRightPressed && _currentSegment)
	{
		_currentSegment->offset.x += 1;

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture x offset: " + DoubleToRoundedString(_currentSegment->offset.x, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyDownPressed && _currentSegment)
	{
		_currentSegment->offset.y += 1;

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture y offset: " + DoubleToRoundedString(-_currentSegment->offset.y, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyCommaPressed && _currentSegment)
	{
		if (_currentSegment->rotation > 0)
		{
			_currentSegment->rotation -= 2.5;
		}

		else
		{
			_currentSegment->rotation = 357.5;
		}

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture rotation: " + DoubleToRoundedString(_currentSegment->rotation, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyPeriodPressed && _currentSegment)
	{
		if (_currentSegment->rotation < 360)
		{
			_currentSegment->rotation += 2.5;
		}

		else
		{
			_currentSegment->rotation = 2.5;
		}

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture rotation: " + DoubleToRoundedString(_currentSegment->rotation, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyPlusPressed && _currentSegment)
	{
		_currentSegment->scale += 0.05;

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture scale: " + DoubleToRoundedString(_currentSegment->scale, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}

	if (_keyMinusPressed && _currentSegment)
	{
		if (_currentSegment->scale > 0.05)
		{
			_currentSegment->scale -= 0.05;
		}

		std::unique_ptr<const Event> event = std::make_unique<const LogWindowMessageEvent>("Segment texture scale: " + DoubleToRoundedString(_currentSegment->scale, 2));
		_services->GetEventHandler()->AddLocalEvent("UI", std::move(event));
	}
}

void Editor::UpdateEditor()
{
	static float holdDownTime = 0;

	if (_keyLeftMouseDown)
	{
		holdDownTime += _services->deltaT;
	}

	else
	{
		holdDownTime = 0;
	}

	if (holdDownTime >= 0.5)
	{
		_follow = true;
	}

	else
	{
		_follow = false;
	}

	// Move joint and its segments
	if (_currentJoint && _follow)
	{
		_currentJoint->position = _cursorPos;

		for (Segment* segment : _segments)
		{
			if (segment->jointTopID == _currentJoint->ID)
			{
				segment->topPos = _currentJoint->position;
				UpdateSegment(*segment);
			}

			if (segment->jointBottomID == _currentJoint->ID)
			{
				segment->bottomPos = _currentJoint->position;
				UpdateSegment(*segment);
			}
		}
	}

	// Move segment
	if (_currentSegment && _follow)
	{
		if (_currentSegment->selectedTop)
		{
			_currentSegment->topPos = _cursorPos;
			UpdateSegment(*_currentSegment);
		}

		else if (_currentSegment->selectedBottom)
		{
			_currentSegment->bottomPos = _cursorPos;
			UpdateSegment(*_currentSegment);
		}
	}
}

void Editor::DrawEditor()
{
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
	DrawLineEx(Vector2{-max, 0}, Vector2{max, 0}, 1, BLACK);
	DrawLineEx(Vector2{0, -max}, Vector2{0, max}, 1, BLACK);

	// Center
	DrawRectangleRec(CenteredRectangle(Rectangle{0, 0, 5, 5}, Vector2{0, 0}), BLACK);

	if (!_play)
	{
		if (_showBack)
		{
			float opacity = (_showBackOpacity / 100.0f) * 255.0f;
			opacity = std::min(opacity, 255.0f);
			_skelAnimation->DrawPasteFrame(_showBackAmount, true, true, true, opacity, opacity / 20);
		}

		else
		{
			_skelAnimation->DrawCurrentFrame(true, true, true, 255);
		}
	}

	else
	{
		_play = !_skelAnimation->DrawAnimation(_loop, _playSpeed);
	}

	EndMode2D();
}

void Editor::Update()
{
	_UI->Update(_skelAnimation.get());

	GetInputs();

	UpdateCamera();
	UpdateSelection();
	UpdateEditorInput();
	UpdateEditor();
}

void Editor::Draw()
{
	DrawEditor();
	_UI->Draw();
}