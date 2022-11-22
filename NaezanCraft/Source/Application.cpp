#include "pch.h"

#include "Application.h"

Application* Application::instance = nullptr;

Application::Application()
{
	craftWindow = Window::CreateCraftWindow();
	//craftWindow->SetEventCallback(std::bind(&Application::OnKeyEvent, this, std::placeholders::_1));
}

void Application::Run()
{
	while (isRunning)
	{
		if (glfwWindowShouldClose(craftWindow->GetWindowObject()))
		{
			isRunning = false;
		}

		craftWindow->Update();
	}
}
