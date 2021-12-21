#define GLFW_INCLUDE_NONE

#include "shader.hpp"
#include "event_registry.hpp"
#include "moving_camera.hpp"

#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include <iostream>

#define DEBUG
#define VERBOSE

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

int main()
{
	const int width = 800;
	const int height = 800;

	// Initialize GLFW
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	GLFWwindow* window = glfwCreateWindow(width, height, "Test", nullptr, nullptr);
	if (!window)
		return EXIT_FAILURE;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	EventRegistry::init(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glErrorCallback, 0);
#endif

	glViewport(0, 0, 800, 800);


	Shader shader("shaders/vtest.glsl", "shaders/ftest.glsl");


	glm::vec3 data[]
	{
		{-1,-1,-5},
		{0,1,-5},
		{1,-1,-5}
	};

	GLuint vao;
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Camera* camera = new MovingCamera(glm::vec3(0), static_cast<float>(width) / height);

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
			case Event::Type::MOUSE_PRESS:
				std::cout << e.pos.x << ", " << e.pos.y << '\n';
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

	delete camera;

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}