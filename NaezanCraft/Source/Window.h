#pragma once

#include <string>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Event/EventSystem.h"

class Window
{
public:
	Window(const std::string& name = "NaezanCraft",
		uint32_t  width = 1280, uint32_t height = 720);
	~Window();

	void Init();
	void Update();
	void Shutdown();

	static std::unique_ptr<Window> CreateCraftWindow(const std::string& name = "NaezanCraft", uint32_t  width = 1280, uint32_t height = 720);

	inline std::string GetWindowName() const { return windowName; }
	inline uint32_t GetWidth() const { return Width; }
	inline uint32_t GetHeight() const { return Height; }
	inline GLFWwindow* GetWindowObject() const { return window; }//static?

	inline void SetEventCallback(const std::function<void(Event&)>& callbackfunc) { EventCallbackFunc = callbackfunc; }

	//TO DO Sync

private:
	std::string windowName;
	uint32_t Width;
	uint32_t Height;
	GLFWwindow* window;
	std::function<void(Event&)> EventCallbackFunc;
};