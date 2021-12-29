#pragma once

#define GLFW_INCLUDE_NONE

#include "defs.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "event_registry.hpp"
#include "moving_camera.hpp"
#include "tesselation_test.hpp"

#include "glad\glad.h"
#include "GLFW\glfw3.h"

class App
{
public:
	App();

	~App();

	int run();

private:
	GLFWwindow* window;

	Shader shader;

	Object* scene;

	//TesselationTest* tesselationTest;
	Camera* camera;

	void initGLFW(int windowWidth, int windowHeight);

	void initGL();

	void init(int windowWidth, int windowHeight);

#ifdef DEBUG
	static void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
};