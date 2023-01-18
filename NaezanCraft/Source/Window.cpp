#include "pch.h"
#include "Window.h"
#include "Input/InputSystem.h"
#include "TextureManager.h"
#include "Sound/SoundManager.h"

using namespace std::placeholders;

Dispatcher Window::eventDispatcher;
uint8_t Window::keyState[249];

Window::Window(const std::string& name, uint32_t  width, uint32_t height)
	: windowName(name), Width(width), Height(height), IsSoundThreadRunning(true)
{
	Init();
}

Window::~Window()
{
	{
		std::lock_guard lock(bgmMutex);
		IsSoundThreadRunning = false;
	}
	bgmThread.join();

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
		NC_LOG_FATAL("Failed to initialize GLAD.");
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, Width, Height);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//위 기능이 있어야지 화이트맵 텍스쳐를 사용할 수 있다
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//gladLoadGL();
	//glfwSwapInterval(1);

	SetEventBindings();
	SetEventCallbacks();

	//std::cout << "Vendor : "  << glGetString(GL_VENDOR) << std::endl;
	//std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
	//std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;

	//텍스쳐 Atlas
	TextureManager::AddTexture("CubeAtlas", "../Assets/Textures/AtlasEdit.png");
	TextureManager::AddTexture("Sun", "../Assets/Textures/Sun.png");
	TextureManager::AddTexture("Moon", "../Assets/Textures/moon_phases.png");
	TextureManager::AddTexture("Cloud", "../Assets/Textures/clouds.png");
	TextureManager::AddTexture("Water", "../Assets/Textures/water.png");
	TextureManager::AddTexture("Crosshair", "../Assets/Textures/crosshair.png");
	TextureManager::AddTexture("InventorySlots", "../Assets/Textures/slots.png");
	TextureManager::AddTexture("InventorySlotPoint", "../Assets/Textures/slotPoint.png");
	TextureManager::AddTexture("InventoryBlockSprite", "../Assets/Textures/InvSprite.png");

	SoundManager::Init();

	SoundManager::AddSound("BGM0", "../Assets/Sounds/Music/Clark.ogg");
	SoundManager::AddSound("BGM1", "../Assets/Sounds/Music/Danny.ogg");
	SoundManager::AddSound("BGM2", "../Assets/Sounds/Music/DryHands.ogg");
	SoundManager::AddSound("BGM3", "../Assets/Sounds/Music/Haggstorm.ogg");
	SoundManager::AddSound("BGM4", "../Assets/Sounds/Music/LivingMice.ogg");
	SoundManager::AddSound("BGM5", "../Assets/Sounds/Music/MiceOnVenus.ogg");
	SoundManager::AddSound("BGM6", "../Assets/Sounds/Music/Minecraft.ogg");
	SoundManager::AddSound("BGM7", "../Assets/Sounds/Music/SubwooferLullaby.ogg");
	SoundManager::AddSound("BGM8", "../Assets/Sounds/Music/Sweden.ogg");
	SoundManager::AddSound("BGM9", "../Assets/Sounds/Music/WetHands.ogg");

	SoundManager::AddSound("cloth1", "../Assets/Sounds/Block/cloth1.ogg");
	SoundManager::AddSound("cloth2", "../Assets/Sounds/Block/cloth2.ogg");
	SoundManager::AddSound("dirt", "../Assets/Sounds/Block/dirt.ogg");
	SoundManager::AddSound("grass1", "../Assets/Sounds/Block/grass1.ogg");
	SoundManager::AddSound("grass2", "../Assets/Sounds/Block/grass2.ogg");
	SoundManager::AddSound("sand1", "../Assets/Sounds/Block/sand1.ogg");
	SoundManager::AddSound("sand2", "../Assets/Sounds/Block/sand2.ogg");
	SoundManager::AddSound("stone1", "../Assets/Sounds/Block/stone1.ogg");
	SoundManager::AddSound("stone2", "../Assets/Sounds/Block/stone2.ogg");
	SoundManager::AddSound("wood1", "../Assets/Sounds/Block/wood1.ogg");
	SoundManager::AddSound("wood2", "../Assets/Sounds/Block/wood2.ogg");

	bgmThread = std::thread(&Window::PlayBGM, this);
}

void Window::Update()
{
	OPTICK_EVENT();

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

void Window::PlayBGM()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(0, 9);
	int curindex = dis(gen);

	while (IsSoundThreadRunning)
	{
		if (!SoundManager::IsPlaying("BGM" + std::to_string(curindex)))
		{
			std::uniform_int_distribution<int> dis1(0, 9);
			curindex = dis1(gen);
			SoundManager::PlayBGM(curindex);
		}
	}
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

	if (Input::GetIsKeyPressed(GLFW_KEY_W) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_W)
	{
		keyState[GLFW_KEY_W] = PRESS;
	}
	if (Input::GetIsKeyReleased(GLFW_KEY_W) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_W)
	{
		keyState[GLFW_KEY_W] = RELEASE;
	}

	if (Input::GetIsKeyPressed(GLFW_KEY_S) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_S)
	{
		keyState[GLFW_KEY_S] = PRESS;
	}
	if (Input::GetIsKeyReleased(GLFW_KEY_S) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_S)
	{
		keyState[GLFW_KEY_S] = RELEASE;
	}

	if (Input::GetIsKeyPressed(GLFW_KEY_A) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_A)
	{
		keyState[GLFW_KEY_A] = PRESS;
	}
	if (Input::GetIsKeyReleased(GLFW_KEY_A) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_A)
	{
		keyState[GLFW_KEY_A] = RELEASE;
	}

	if (Input::GetIsKeyPressed(GLFW_KEY_D) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_D)
	{
		keyState[GLFW_KEY_D] = PRESS;
	}
	if (Input::GetIsKeyReleased(GLFW_KEY_D) && EventTypeCast<KeyEvent>(event).Getkey() == GLFW_KEY_D)
	{
		keyState[GLFW_KEY_D] = RELEASE;
	}

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

bool Window::IsKeyPressed(int keycode)
{
	if (keyState[keycode] == PRESS)
		return true;
	return false;
}

bool Window::IsKeyReleased(int keycode)
{
	if (keyState[keycode] == RELEASE)
	{
		keyState[keycode] = DEFAULT;
		return true;
	}
	return false;
}
