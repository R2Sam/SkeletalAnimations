#pragma once
#include "MyRaylib.h"
#include "log.h"
#include "Event.h"

class Services;
class SceneHandler;

class Game : public EventListener
{
private:

	void Init();
	void DeInit();

	void AddSelfAsListener() override;

	// Run once per frame
	void Tick();

	void Update();
	void Draw();

	void OnEvent(std::shared_ptr<const Event>& event) override;

	bool GameShouldClose();

	bool _shouldClose = false;
	
	// Basic game components 
	std::unique_ptr<Services> _services;
	Services* _servicesPtr;
	std::unique_ptr<SceneHandler> _sceneHandler;

public:

	Game(int width, int height, const std::string title);
	~Game();

	void Run();
};

class GameCloseEvent : public Event
{
	
};