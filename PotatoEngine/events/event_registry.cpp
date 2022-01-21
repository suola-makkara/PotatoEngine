#include "event_registry.hpp"

#include <iostream>

void EventRegistry::init(GLFWwindow* window)
{
	glfwSetErrorCallback(errorCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCharCallback(window, characterCallback);
}

bool EventRegistry::hasNextEvent()
{
	return !events.empty();
}

Event EventRegistry::getNext()
{
	if (!hasNextEvent())
	{
		Event e{};
		e.type = Event::Type::UNKNOWN;
		return e;
	}
	else
	{
		Event e = events.front();
		events.pop();
		return e;
	}
}


void EventRegistry::errorCallback(int error, const char* description)
{
	std::cout << "GLFW Error: " << description << '\n';
}

void EventRegistry::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Event::Type type = Event::Type::UNKNOWN;
	switch (action)
	{
	case GLFW_PRESS:
		type = Event::Type::KEY_PRESS;
		break;
	case GLFW_REPEAT:
		type = Event::Type::KEY_REPEAT;
		break;
	case GLFW_RELEASE:
		type = Event::Type::KEY_RELEASE;
		break;
	}

	if (type != Event::Type::UNKNOWN)
		events.push(Event(type, key, mods));
}

void EventRegistry::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	events.push(Event(Event::Type::MOUSE_MOVE, glm::dvec2(xpos, ypos)));
}

void EventRegistry::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	Event::Type type = Event::Type::UNKNOWN;
	switch (action)
	{
	case GLFW_PRESS:
		type = Event::Type::MOUSE_PRESS;
		break;
	case GLFW_RELEASE:
		type = Event::Type::MOUSE_RELEASE;
		break;
	}

	if (type != Event::Type::UNKNOWN)
		events.push(Event(type, button, glm::dvec2(x, y), mods));
}

void EventRegistry::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	events.push(Event(Event::Type::SCROLL, yoffset));
}

void EventRegistry::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	events.push(Event(Event::Type::FRAME_BUFFER_RESIZE, glm::ivec2(width, height)));
}

void EventRegistry::characterCallback(GLFWwindow* window, unsigned character)
{
	events.push(Event(Event::Type::CHAR, character));
}

std::queue<Event> EventRegistry::events = std::queue<Event>();