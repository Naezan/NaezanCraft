#pragma once

#include "Window.h"
#include <memory>

//TO DO move to position ob macro function
#define GET_WINDOW_OBJECT() Application::GetInstance()->GetWindow()->GetWindowObject()

class Application
{
public:
	Application();
	~Application() = default;

	void Run();

	inline static Application* GetInstance()
	{
		if(instance == nullptr)
			instance = new Application();
		return instance;
	}

	inline static std::unique_ptr<Window>& GetWindow() { return instance->craftWindow; }

private:
	std::unique_ptr<Window> craftWindow;
	bool isRunning = true;

	static Application* instance;
};
