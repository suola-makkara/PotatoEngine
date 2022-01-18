#pragma once

#define GLFW_INCLUDE_NONE

#include "defs.hpp"
#include "editor.hpp"
#include "shader.hpp"
#include "event_registry.hpp"
#include "moving_camera.hpp"
#include "editor.hpp"

#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include <memory>

class App
{
private:
	friend int main();

	App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;

	~App();

	int run();

	void initGLFW(int windowWidth, int windowHeight);

	void initGL();

	void init(int windowWidth, int windowHeight);

	GLFWwindow* window;
	std::unique_ptr<Editor> editor;

#ifdef DEBUG
	static void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
};