#include "app.hpp"

#include <iostream>

App::App()
{
	const int windowWidth = 800;
	const int windowHeight = 800;
	initGLFW(windowWidth, windowHeight);
	initGL();
	init(windowWidth, windowHeight);
}

App::~App()
{
	delete tesselationTest;

	delete camera;

	glfwDestroyWindow(window);
	glfwTerminate();
}

int App::run()
{
	double time = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double tempTime = glfwGetTime();
		double dt = tempTime - time;
		time = tempTime;

		//std::cout << "fps: " << static_cast<int>(glm::round(1.0 / dt)) << '\n';

		glfwPollEvents();
		while (EventRegistry::hasNextEvent())
		{
			Event e = EventRegistry::getNext();
			switch (e.type)
			{
			case Event::Type::FRAME_BUFFER_RESIZE:
				glViewport(0, 0, e.size.x, e.size.y);
				break;
			case Event::Type::KEY_PRESS:
				if (e.key == GLFW_KEY_R)
					tesselationTest->shader.compile();
				break;
			}

			camera->handleEvent(e);
		}

		camera->update(static_cast<float>(dt));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tesselationTest->render(camera);

		glfwSwapBuffers(window);
	}

	return EXIT_SUCCESS;
}

void App::initGLFW(int windowWidth, int windowHeight)
{
	if (!glfwInit())
		return exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	window = glfwCreateWindow(windowWidth, windowHeight, "Potato Engine", nullptr, nullptr);
	if (!window)
		return exit(EXIT_FAILURE);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
}

void App::initGL()
{
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glErrorCallback, 0);
#endif

	glViewport(0, 0, 800, 800);
}

void App::init(int windowWidth, int windowHeight)
{
	EventRegistry::init(window);

	tesselationTest = new TesselationTest();

	camera = new MovingCamera(glm::vec3(25,30,25), static_cast<float>(windowWidth) / windowHeight);
	dynamic_cast<MovingCamera*>(camera)->far = 300.0f;
}

#ifdef DEBUG
#include <deque>
void GLAPIENTRY App::glErrorCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	static std::deque<std::string> messageDeque;

	if (SUPPRESS_GL_REPEAT > 0)
	{
		bool repeat = false;
		for (const auto& prevMessage : messageDeque)
			if (prevMessage == message)
			{
				repeat = true;
				break;
			}

		messageDeque.push_back(message);
		if (messageDeque.size() > SUPPRESS_GL_REPEAT)
			messageDeque.pop_front();

		if (repeat) return;
	}

	if (type == GL_DEBUG_TYPE_ERROR)
		std::cout << "GL Error: " << message << '\n';
#ifdef VERBOSE
	else
		std::cout << "GL Other: " << message << '\n';
#endif
}
#endif