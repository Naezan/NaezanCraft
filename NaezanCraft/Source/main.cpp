#include "pch.h"
#include "Application.h"

int main(int argc, char** argv)
{
	MessageLog::Initialize();

	auto app = new Application();
	app->Run();
	delete app;

	return 0;
}