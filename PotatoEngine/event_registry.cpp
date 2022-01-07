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
	Event e{};
	switch (action)
	{
	case GLFW_PRESS:
		e.type = Event::Type::KEY_PRESS;
		break;
	case GLFW_REPEAT:
		e.type = Event::Type::KEY_REPEAT;
		break;
	case GLFW_RELEASE:
		e.type = Event::Type::KEY_RELEASE;
		break;
	default:
		e.type = Event::Type::UNKNOWN;
	}

	e.key = key;

	events.push(e);
}

void EventRegistry::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	Event e{};
	e.type = Event::Type::MOUSE_MOVE;
	e.pos = glm::dvec2(xpos, ypos);

	events.push(e);
}

void EventRegistry::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	Event e{};
	switch (action)
	{
	case GLFW_PRESS:
		e.type = Event::Type::MOUSE_PRESS;
		break;
	case GLFW_RELEASE:
		e.type = Event::Type::MOUSE_RELEASE;
		break;
	default:
		e.type = Event::Type::UNKNOWN;
		break;
	}

	e.pos = glm::dvec2(x, y);
	e.button = button;

	events.push(e);
}

void EventRegistry::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Event e{};
	e.type = Event::Type::SCROLL;
	e.scroll = yoffset;

	events.push(e);
}

void EventRegistry::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Event e{};
	e.type = Event::Type::FRAME_BUFFER_RESIZE;
	e.size = glm::ivec2(width, height);

	events.push(e);
}

void EventRegistry::characterCallback(GLFWwindow* window, unsigned character)
{
	Event e{};
	e.type = Event::Type::CHAR;
	e.character = character;
	
	events.push(e);
}

std::queue<Event> EventRegistry::events = std::queue<Event>();