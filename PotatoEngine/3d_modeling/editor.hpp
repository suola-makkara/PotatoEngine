#pragma once

#include "event.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "command.hpp"
#include "editor_context.hpp"
#include "editor_draw_utils.hpp"
#include "config.hpp"
#include "noncopyable.hpp"
#include "script_runner.hpp"

#include "GLFW/glfw3.h"

#include <memory>
#include <stack>

class Editor : Noncopyable
{
private:
	friend class App;

	Editor(GLFWwindow* window);

	void handleEvent(const Event& event);

	void update(float dt);

	void render();

	EditorContext context;

	void submitCommand(const std::string& commandString);

	std::unique_ptr<Camera> camera;

	Shader shader;
	Shader wireframeShader;

	GLFWwindow* window;
	glm::ivec2 windowSize;

	Config config;

	ScriptRunner sr;

	//enum class EditMode
	//{
	//	OBJECT,
	//	VERTEX,
	//};

	//EditMode editMode = EditMode::OBJECT;

	enum class Mode
	{
		NONE,
		AREA_SELECT,
		COMMAND_ENTER,
		MOVE_X,
		MOVE_Y,
		MOVE_Z,
		MOVE_XY,
		MOVE_YZ,
		MOVE_ZX,
		ROTATE,
		ROTATE_X,
		ROTATE_Y,
		ROTATE_Z,
		SCALE,
		SCALE_X,
		SCALE_Y,
		SCALE_Z,
		SCALE_XY,
		SCALE_YZ,
		SCALE_ZX,
	};

	struct
	{
		glm::ivec2 start;
		glm::ivec2 end;
	} areaSelect;

	glm::vec3 startPosition;
	glm::mat3 startBasis;
	glm::vec3 startScale;

	std::string command;

	Mode mode = Mode::NONE;

	void setMode(Mode mode);

	//void setEditMode(EditMode mode);

	std::list<Object::VertexRef> selectedVertices{};

	void selectObject(Object* object);
	void unselectObject();
	Object* selectedObject = nullptr;

	void updateObjectTransform(const glm::dvec2& mousePos);

	glm::vec2 screenToNDC(const glm::ivec2& v) const;

	void executeCommand();

	glm::dvec2 getMousePos();

	bool isMoveMode(Mode mode) const;
	bool isRotationMode(Mode mode) const;
	bool isScaleMode(Mode mode) const;

	bool leftCtrlDown() const;
	bool leftShiftDown() const;

	std::unique_ptr<EditorDrawUtils> drawUtils;
};