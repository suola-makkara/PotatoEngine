#include "app.hpp"

#include <iostream>

#ifdef DEBUG
void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
		std::cout << "GL Error: " << message;
#ifdef VERBOSE
	else
		std::cout << "GL Other: " << message;
#endif
}
#endif

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
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

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
					shader.compile();
				break;
			}

			camera->handleEvent(e);
		}

		camera->update(static_cast<float>(dt));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.set("uView", camera->getViewMat());
		shader.set("uProj", camera->getProjMat());

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

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

	shader = Shader("shaders/vtest.glsl", "shaders/ftest.glsl");

	glm::vec3 data[]
	{
		{-1,-1,-5},
		{0,1,-5},
		{1,-1,-5}
	};

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	camera = new MovingCamera(glm::vec3(0), static_cast<float>(windowWidth) / windowHeight);
}