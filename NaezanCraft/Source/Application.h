#pragma once

#include "Window.h"
#include "World/World.h"

class Application
{
public:
	Application();
	~Application() = default;

	void Run();

	inline static Application* GetInstance()
	{
		if (instance == nullptr)
			instance = new Application();
		return instance;
	}

	inline static std::unique_ptr<Window>& GetWindow() { return instance->craftWindow; }
	inline static std::unique_ptr<World>& GetWorld() { return instance->craftWorld; }

private:
	std::unique_ptr<Window> craftWindow;
	std::unique_ptr<World> craftWorld;
	bool isRunning = true;

	static Application* instance;
};

//TO DO move to position ob macro function
#define GET_WINDOW_OBJECT() Application::GetInstance()->GetWindow()->GetWindowObject()
#define GET_World() Application::GetInstance()->GetWorld()