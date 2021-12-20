#include "shader.hpp"

#include "glad\glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW\glfw3.h"

#include <iostream>

#define DEBUG

#ifdef DEBUG
void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
		std::cout << "GL Error: " << message;
	else
		std::cout << "GL Other: " << message;
}
#endif

void errorCallback(int error, const char* description);

int main()
{
	// Initialize GLFW
	if (!glfwInit())
		return EXIT_FAILURE;

	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Test", nullptr, nullptr);
	if (!window)
		return EXIT_FAILURE;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glErrorCallback, 0);
#endif

	glViewport(0, 0, 800, 800);



	Shader shader("shaders/vtest.glsl", "shaders/ftest.glsl");


	glm::vec3 data[]
	{
		{0,0,0},
		{0,1,0},
		{1,0,0}
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

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

void errorCallback(int error, const char* description)
{
	std::cout << "GLFW: " << description << '\n';
}