#include "EditorScene.h"
#include "Services.h"
#include "EventHandler.h"
#include "SceneHandler.h"

#include "Editor.h"
#include "UI.h"

EditorScene::EditorScene(Services* servicesIn) : _services(servicesIn)
{
	AddSelfAsListener();
	Init();
}

EditorScene::~EditorScene()
{
	_services->GetEventHandler()->RemoveListener(_ptr);
}

void EditorScene::Init()
{
	_editor = std::make_unique<Editor>(_services);
}

void EditorScene::AddSelfAsListener()
{
	// Groups to listen to
	_services->GetEventHandler()->AddListener(_ptr);
	_services->GetEventHandler()->AddLocalListener("MainLevel" ,_ptr);
}

void EditorScene::OnEvent(std::shared_ptr<const Event>& event)
{
	// If not active ignore events
	if (!_active)
		return;
}

void EditorScene::GetInputs()
{
	// Get keys pressed
	_keyEscapePressed = IsKeyPressed(KEY_ESCAPE);
}

void EditorScene::Enter()
{
	_active = true;
}

void EditorScene::Exit()
{
	_active = false;
}



void EditorScene::Update()
{
	if(!_active)
		return;

	GetInputs();

	// Exit scene and go to menu
	if (_keyEscapePressed && _active)
	{
		std::unique_ptr<const Event> event = std::make_unique<const ClosingEvent>();
		_services->GetEventHandler()->AddLocalEvent("SceneHandler", std::move(event));

		_keyEscapePressed = false;
		_active = false;
		return;
	}

	_editor->Update();
}

void EditorScene::Draw()
{
	if(!_active)
		return;

	_editor->Draw();
}