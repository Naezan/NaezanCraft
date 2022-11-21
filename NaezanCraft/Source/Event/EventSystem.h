#pragma once

#include <map>
#include <functional>

/*
* header name might be singleton but is not, my mistake
*/

enum class EventType
{
	WindowPos, WindowSize, WindowClose, WindowMax,
	Key, MouseButton, CursorPos, Scroll
};

#define DEF_EVENT_TYPE(type) virtual const inline EventType GetEventType() const override { return type; }

class Event;

template<typename T>
const T& EventTypeCast(const Event& e)
{
	return static_cast<const T&>(e);
}

class Event
{
public:
	virtual ~Event() = default;

	virtual const EventType GetEventType() const = 0;

	const static inline std::string ToString(EventType type)
	{
		switch (type)
		{
		case EventType::WindowPos: return "WindowPos";
		case EventType::WindowSize: return "WindowSize";
		case EventType::WindowClose: return "WindowClose";
		case EventType::WindowMax: return "WindowMax";
		case EventType::Key: return "key";
		case EventType::MouseButton: return "MouseButton";
		case EventType::CursorPos: return "CursorPos";
		case EventType::Scroll: return "Scroll";
		}
		return "Event is Null";
	}
};

class WindowPosEvent : public Event
{
public:
	WindowPosEvent(int _xpos, int _ypos) : xpos(_xpos), ypos(_ypos) {}

	inline int Getxpos() const { return xpos; }
	inline int Getypos() const { return ypos; }

	DEF_EVENT_TYPE(EventType::WindowPos)

private:
	int xpos, ypos;
};

class WindowSizeEvent : public Event
{
public:
	WindowSizeEvent(int _width, int _height) : width(_width), height(_height) {}

	inline int Getwidth() const { return width; }
	inline int Getheight() const { return height; }

	DEF_EVENT_TYPE(EventType::WindowSize)

private:
	int width, height;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;

	DEF_EVENT_TYPE(EventType::WindowClose)

private:
};

class WindowMaxEvent : public Event
{
public:
	WindowMaxEvent(int _maximized) : maximized(_maximized) {}

	inline int GetMaximized() const { return maximized; }

	DEF_EVENT_TYPE(EventType::WindowMax)

private:
	int maximized;
};

class KeyEvent : public Event
{
public:
	KeyEvent(int _key, int _scancode, int _action, int _mods) : key(_key), scancode(_scancode), action(_action), mods(_mods) {}

	DEF_EVENT_TYPE(EventType::Key)

	inline int Getkey() const { return key; }
	inline int Getscancode() const { return scancode; }
	inline int Getaction() const { return action; }
	inline int Getmods() const { return mods; }

private:
	int key, scancode, action, mods;
};

class MouseButtonEvent : public Event
{
public:
	MouseButtonEvent(int _button, int _action, int _mods) : button(_button), action(_action), mods(_mods) {}

	inline int Getbutton() const { return button; }
	inline int Getaction() const { return action; }
	inline int Getmods() const { return mods; }

	DEF_EVENT_TYPE(EventType::MouseButton)

private:
	int button, action, mods;
};

class CursorPosEvent : public Event
{
public:
	CursorPosEvent(double _xpos, double _ypos) : xpos(_xpos), ypos(_ypos) {}

	inline double Getxpos() const { return xpos; }
	inline double Getypos() const { return ypos; }

	DEF_EVENT_TYPE(EventType::CursorPos)

private:
	double xpos, ypos;
};

class ScrollEvent : public Event
{
public:
	ScrollEvent(double _xoffset, double _yoffset) : xoffset(_xoffset), yoffset(_yoffset) {}

	inline double Getxoffset() const { return xoffset; }
	inline double Getyoffset() const { return yoffset; }

	DEF_EVENT_TYPE(EventType::Scroll)

private:
	double xoffset, yoffset;
};

class Dispatcher
{
public:
	using EventFunc = std::function< void(const Event&)>;

	void AddCallbackFunction(const EventType&& type, EventFunc&& eventfunc)
	{
		eventCallbackMap.emplace(type, eventfunc);
	}

	void PostCallbackFunction(const Event& event)
	{
		EventType type = event.GetEventType();

		if (eventCallbackMap.find(type) == eventCallbackMap.end())
			return;

		EventFunc& callbackfunc = eventCallbackMap.at(type);
		callbackfunc(event);
	}

private:
	//TO DO maybe Eventfunc will be array, vector or something?
	std::map<EventType, EventFunc> eventCallbackMap;
};