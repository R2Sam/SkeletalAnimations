#pragma once
#include "log.h"
#include "MyRaylib.h"
#include "Scene.h"
#include "Event.h"

class Services;
class Editor;

class EditorScene : public Scene, public EventListener
{
private:

	// Ptr of global services
	Services* _services;

	// Keys
	bool _keyEscapePressed;

	std::unique_ptr<Editor> _editor;

	void Init() override;

	void AddSelfAsListener() override;
	void OnEvent(std::shared_ptr<const Event>& event) override;

	void GetInputs() override;

public:

	EditorScene(Services* servicesIn);
	~EditorScene();

	void Enter() override;
	void Exit() override;

	void Update() override;
	void Draw() override;
};