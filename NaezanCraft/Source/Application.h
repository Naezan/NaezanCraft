#pragma once

#include "Window.h"
#include <memory>

//Singleton?
class Application
{
public:
	Application();
	~Application();

	void Run();

	void OnKeyEvent(Event& event);

private:
	std::unique_ptr<Window> craftWindow;
	bool isRunning = true;
};
