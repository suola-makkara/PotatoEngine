#pragma once

#include "event.hpp"
#include "object.hpp"
#include "shader.hpp"

#include "GLFW/glfw3.h"

class Editor
{
public:
	void handleEvent(const Event& event);

	void update(float dt);

	void render();

	static Editor& get(GLFWwindow* window);

private:
	Object* scene;
	Shader shader;
	Camera* camera;

	glm::ivec2 windowSize;

	enum class Mode
	{
		NONE,
		AREA_SELECT,
		COMMAND_ENTER,
	};

	struct
	{
		glm::ivec2 start;
		glm::ivec2 end;
	} areaSelect;

	std::string command;

	Mode mode = Mode::NONE;

	void setMode(Mode mode);

	std::list<Object::VertexRef> selectedVertices{};
	Object* selectedObject = nullptr;

	Editor(GLFWwindow* window);
	Editor(const Editor&) = delete;
	Editor(Editor&&) = delete;
	Editor& operator=(const Editor&) = delete;
	Editor& operator=(Editor&&) = delete;

	~Editor();

	glm::vec2 screenToNDC(const glm::ivec2& v) const;

	Ray castRay(const glm::vec2& coord) const;
};