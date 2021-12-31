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
	};

	union
	{
		struct
		{
			glm::ivec2 start;
			glm::ivec2 end;
		} areaSelect;
	};

	Mode mode = Mode::NONE;

	std::list<Object::VertexRef> selectedVertices{};

	Editor(GLFWwindow* window);
	Editor(const Editor&) = delete;
	Editor(Editor&&) = delete;
	Editor& operator=(const Editor&) = delete;
	Editor& operator=(Editor&&) = delete;

	~Editor();

	glm::vec2 screenToNDC(const glm::ivec2& v);
};