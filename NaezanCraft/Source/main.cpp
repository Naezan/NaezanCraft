#include "pch.h"
#include "Application.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//This software uses code from Hazel under the Apache-2.0 license.
//I don't know how detail about license, Do I need a used Header and CPP?

int main(int argc, char** argv)
{
	MessageLog::Initialize();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(155);

	Application* appInstance = Application::GetInstance();
	appInstance->Run();
	delete appInstance;

	_CrtDumpMemoryLeaks();

	return 0;
}