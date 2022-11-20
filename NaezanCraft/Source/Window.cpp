#include "pch.h"
#include "Window.h"

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

	SetEventBinding();
	SetEventCallbacks();
}

void Window::Update()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Shutdown()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::SetEventBinding()
{
	//TO DO modify when funtion imple
	eventDispatcher.AddCallbackFunction(EventType::WindowPos, std::bind(&Window::OnWindowPos, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowSize, std::bind(&Window::OnWindowSize, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowClose, std::bind(&Window::OnWindowClose, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::WindowMax, std::bind(&Window::OnWindowMaximize, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::Key, std::bind(&Window::OnKey, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::MouseButton, std::bind(&Window::OnMouseButton, this, _1));
	eventDispatcher.AddCallbackFunction(EventType::CursorPos, std::bind(&Window::OnCursorPos, this, _1));
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
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnWindowSize(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnWindowClose(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnWindowMaximize(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnKey(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnMouseButton(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnCursorPos(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}

void Window::OnScroll(const Event& event)
{
	NC_LOG_DEBUG("'{0}'", event.ToString(event.GetEventType()));
}