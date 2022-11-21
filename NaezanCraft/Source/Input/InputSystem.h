#pragma once

//I don't using Input Polling System, I'll using Input Event System
//This is Test Input Polling System.

#include <GLFW/glfw3.h>
#include "../Application.h"

//TO DO something key enumtype

class Input
{
public:
	static bool GetIsKeyPressed(const int keycode)
	{
		GLFWwindow* window = GET_WINDOW_OBJECT();
		return glfwGetKey(window, keycode) == GLFW_PRESS;
	}
	static bool GetIsKeyRepeating(const int keycode)
	{
		GLFWwindow* window = GET_WINDOW_OBJECT();
		return glfwGetKey(window, keycode) == GLFW_REPEAT;
	}
	static bool GetIsKeyReleased(const int keycode)
	{
		GLFWwindow* window = GET_WINDOW_OBJECT();
		return glfwGetKey(window, keycode) == GLFW_RELEASE;
	}
	static bool GetIsMouseButtonPressed(const int button)
	{
		GLFWwindow* window = GET_WINDOW_OBJECT();
		glfwGetMouseButton(window, button);
	}
	static bool GetIsMouseButtonReleased(const int button)
	{
		GLFWwindow* window = GET_WINDOW_OBJECT();
		glfwGetMouseButton(window, button);
	}
private:

};