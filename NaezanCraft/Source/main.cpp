#include "pch.h"
#include "Application.h"

//This software uses code from Hazel under the Apache-2.0 license.
//I don't know how detail about license, Do I need a used Header and CPP?

int main(int argc, char** argv)
{
	MessageLog::Initialize();

	Application* appInstance = Application::GetInstance();
	appInstance->Run();
	delete appInstance;

	return 0;
}