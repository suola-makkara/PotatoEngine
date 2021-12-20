#include "glad\glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW\glfw3.h"

//#include "glm\vec3.hpp"

#include <iostream>

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

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

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