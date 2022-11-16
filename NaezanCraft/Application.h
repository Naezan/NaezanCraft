#pragma once

#include "config.h"

//Singleton
class ENGINE_API Application
{
public:
	Application();
	~Application();

	void Run();
};

