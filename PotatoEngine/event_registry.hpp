#pragma once

#include "event.hpp"

#include "GLFW\glfw3.h"

#include <queue>

// wrapper class for GLFW event callbacks
class EventRegistry
{
public:
	static void init(GLFWwindow* window);

	static bool hasNextEvent();

	static Event getNext();

private:
	static std::queue<Event> events;

	static void errorCallback(int error, const char* description);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};