#pragma once

#include <map>
#include <functional>

enum class EventType
{
	KeyPressed, KeyReleased, MousePressed, MouseReleased, MouseMove
};

class Event
{
public:
	virtual ~Event() {}


private:
	EventType eventType;

};

class Dispatcher
{
public:


private:
	std::map<EventType, std::function< void(const Event&)>> eventCallbackMap;
};