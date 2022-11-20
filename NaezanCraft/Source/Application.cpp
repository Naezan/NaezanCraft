#include "pch.h"

#include "Application.h"

Application::Application()
{
	craftWindow = Window::CreateCraftWindow();
	//craftWindow->SetEventCallback(std::bind(&Application::OnKeyEvent, this, std::placeholders::_1));
}

Application::~Application()
{
}

void Application::Run()
{
	while (isRunning)
	{
		craftWindow->Update();
	}
}
