#include "pch.h"

#include "Application.h"
#include "Time/Timer.h"

Application* Application::instance = nullptr;

Application::Application()
{
	craftWindow = Window::CreateCraftWindow();
	craftWorld = World::CreateCraftWorld();
	//craftWindow->SetEventCallback(std::bind(&Application::OnKeyEvent, this, std::placeholders::_1));
}

void Application::Run()
{
	while (isRunning)
	{
		OPTICK_FRAME("MainThread");

		if (glfwWindowShouldClose(craftWindow->GetWindowObject()))
		{
			isRunning = false;
		}

		Timer::MeasuringFrameTime();

		craftWindow->Update();
		craftWorld->Update();

		craftWorld->Render();
	}
}
