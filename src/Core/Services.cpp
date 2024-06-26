#include "Services.h"
#include "EventHandler.h"
#include "GameStateHandler.h"

Services::Services()
{
	Init();
}

Services::~Services()
{

}

void Services::Init()
{
	// Create event and game handler
	_eventHandler = std::make_unique<EventHandler>();
	_gameStateHandler = std::make_unique<GameStateHandler>(this);

	// Get inital values
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();
}

void Services::UpdateVar()
{
	// Update vars
	deltaT = GetFrameTime();
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();
}

void Services::UpdateObj()
{
	// Update any other objects
	_gameStateHandler->Update();
}

EventHandler* Services::GetEventHandler()
{
	// Give out the event handler
	return _eventHandler.get();
}

GameStateHandler* Services::GetGameStateHandler()
{
	return _gameStateHandler.get();
}