#include "../pch.h"
#include "Timer.h"
#include <GLFW/glfw3.h>

double Timer::frameTime;

void Timer::MeasuringFrameTime()
{
	static double fps_start = glfwGetTime();
	static unsigned int fps_frames = 0;
	fps_frames++;
	double fps_end = glfwGetTime();
	if (fps_end - fps_start >= 1.0) {
		frameTime = (1000.0 / (double)fps_frames);
		//std::cout << (1000.0 / (double)fps_frames) << "ms" << std::endl;
		fps_frames = 0.0;
		fps_start = fps_end;
	}
}
