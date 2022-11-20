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

private:
	std::unique_ptr<Window> craftWindow;
	bool isRunning = true;
};
