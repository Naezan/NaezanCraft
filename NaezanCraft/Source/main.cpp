#include "pch.h"
#include "Application.h"

int main(int argc, char** argv)
{
	MessageLog::Initialize();

	Application* appInstance = Application::GetInstance();
	appInstance->Run();
	delete appInstance;

	return 0;
}