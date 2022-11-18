#include "pch.h"
#include "Window.h"

Window::Window(const std::string& name, uint32_t  width, uint32_t height)
	: windowName(name), Width(width), Height(height)
{
	Init();
}

Window::~Window()
{
	Shutdown();
}

void Window::Init()
{
	//glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(Width, Height, windowName.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//TO DO register EventCallback
	//glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	//gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//glfwSwapInterval(1);
}

void Window::Update()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
	//TO DO
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Shutdown()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::unique_ptr<Window> Window::CreateCraftWindow(const std::string& name, uint32_t  width, uint32_t height)
{
	return std::make_unique<Window>(name, width, height);
}