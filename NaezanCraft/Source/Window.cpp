#include "pch.h"
#include <glad/glad.h>
#include "Window.h"
#include "Input/InputSystem.h"

using namespace std::placeholders;

Dispatcher Window::eventDispatcher;

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
	if (!glfwInit())
	{
		NC_LOG_FATAL("Fail to Initailize glfw.");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Do not use Fixed Function Pipeline
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(Width, Height, windowName.c_str(), NULL, NULL);
	if (!window)
	{
		NC_LOG_FATAL("Fail to Create Window Object.");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	//setup glad depends on OS? In this case we are using window 10
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		NC_LOG_FATAL("Fail to Load Glad.");
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, Width, Height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//gladLoadGL();
	//glfwSwapInterval(1);

	SetEventBindings();
	SetEventCallbacks();

	//std::cout << "Vendor : "  << glGetString(GL_VENDOR) << std::endl;
	//std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
	//std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
}

void Window::Update()
{
	glfwPollEvents();
	//double buffering
	glfwSwapBuffers(window);
}

void Window::Shutdown()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::SetEventBindings()
{
	//TO DO modify when funtion imple class pos
	eventDispatcher.AddCallbackFunction(EventType::WindowPos, std::bind(&Window::OnWindowPos, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowSize, std::bind(&Window::OnWindowSize, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowClose, std::bind(&Window::OnWindowClose, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowMax, std::bind(&Window::OnWindowMaximize, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::FramebufferSize, std::bind(&Window::OnFramebufferSize, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::Key, std::bind(&Window::OnKey, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::MouseButton, std::bind(&Window::OnMouseButton, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::Scroll, std::bind(&Window::OnScroll, this, _1));
}

void Window::SetEventCallbacks()
{
	//윈도우 움직임
	glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos)
		{
			eventDispatcher.PostCallbackFunction(WindowPosEvent(xpos, ypos));
		});

	//리사이징
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			eventDispatcher.PostCallbackFunction(WindowSizeEvent(width, height));
		});

	//윈도우 닫기
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			eventDispatcher.PostCallbackFunction(WindowCloseEvent());
		});

	//윈도우 전체화면
	glfwSetWindowMaximizeCallback(window, [](GLFWwindow* window, int maximized)
		{
			eventDispatcher.PostCallbackFunction(WindowMaxEvent(maximized));
		});

	//프레임버퍼 바뀔때마다?
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			eventDispatcher.PostCallbackFunction(FramebufferSizeEvent(width, height));
		});

	// when a key is pressed, repeated or released.키프레스, 릴리즈, 리피트
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			eventDispatcher.PostCallbackFunction(KeyEvent(key, scancode, action, mods));
		});

	// which is called when a mouse button is pressed or released.마우스 프레스, 릴리즈
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			eventDispatcher.PostCallbackFunction(MouseButtonEvent(button, action, mods));
		});

	//  which is called when the cursor is moved 마우스 무브
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
		{
			eventDispatcher.PostCallbackFunction(CursorPosEvent(xpos, ypos));
		});

	// which is called when a scrolling device is used 마우스 스크롤
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			eventDispatcher.PostCallbackFunction(ScrollEvent(xoffset, yoffset));
		});
}

void Window::OnWindowPos(const Event& event)
{
	//auto& e = EventTypeCast<WindowPosEvent>(event);
	//NC_LOG_DEBUG("'xpos : {0}' 'ypos : {1}'", e.Getxpos(), e.Getypos());
}

void Window::OnWindowSize(const Event& event)
{
	//const WindowSizeEvent& e = EventTypeCast<WindowSizeEvent>(event);
	//glViewport(0, 0, e.Getwidth(), e.Getheight());
	//NC_LOG_DEBUG("'xpos : {0}' 'ypos : {1}'", e.Getwidth(), e.Getheight());
}

void Window::OnWindowClose(const Event& event)
{
	//NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnWindowMaximize(const Event& event)
{
	//NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnFramebufferSize(const Event& event)
{
	const FramebufferSizeEvent& e = EventTypeCast<FramebufferSizeEvent>(event);
	glViewport(0, 0, e.Getwidth(), e.Getheight());
	//NC_LOG_DEBUG("'xpos : {0}' 'ypos : {1}'", e.Getwidth(), e.Getheight());
}

void Window::OnKey(const Event& event)
{
	if (Input::GetIsKeyPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);


	//NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnMouseButton(const Event& event)
{
	//NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnScroll(const Event& event)
{
	//NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}