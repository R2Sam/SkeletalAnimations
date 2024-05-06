#pragma once
#include "log.h"
#include "Event.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <map>

class EventHandler
{
private:

	// List of all listeners
	std::vector<EventListener*> _listeners;

	// List of listeners for local queues
	std::map<std::string, std::vector<EventListener*>> _localListeners;

	void ProcessEvent(std::unique_ptr<const Event> event);

	void ProcessEventLocal(const std::string name, std::unique_ptr<const Event> event);

	void RemoveLocalListener(EventListener* listener);

public:

	EventHandler();
	~EventHandler();

	void AddEvent(std::unique_ptr<const Event> event);
	void AddListener(EventListener* listener);
	void RemoveListener(EventListener* listener);

	void AddLocalEvent(const std::string name, std::unique_ptr<const Event> event);
	void AddLocalListener(const std::string name, EventListener* listener);
};