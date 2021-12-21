#pragma once

#define GLFW_INCLUDE_NONE

#include "shader.hpp"
#include "event_registry.hpp"
#include "moving_camera.hpp"

#include "glad\glad.h"
#include "GLFW\glfw3.h"

#define DEBUG
#define VERBOSE

class App
{
public:
	App();

	~App();

	int run();

private:
	GLFWwindow* window;

	Shader shader;
	Camera* camera;

	GLuint vbo;
	GLuint vao;

	void initGLFW(int windowWidth, int windowHeight);

	void initGL();

	void init(int windowWidth, int windowHeight);
};