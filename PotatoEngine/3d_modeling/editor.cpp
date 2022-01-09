#include "editor.hpp"
#include "mesh.hpp"
#include "moving_camera.hpp"
#include "editor_draw_utils.hpp"
#include "defs.hpp"
#include "ray.hpp"
#include "ray_cast.hpp"
#include "ray_utiles.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

void Editor::handleEvent(const Event& event)
{
	if (mode != Mode::COMMAND_ENTER)
		camera->handleEvent(event);

	switch (event.type)
	{
	case Event::Type::FRAME_BUFFER_RESIZE:
		windowSize = event.size;
		break;
	case Event::Type::KEY_RELEASE:
		switch (event.key)
		{
		case GLFW_KEY_ESCAPE:
			setMode(Mode::NONE);
			selectedVertices.clear();
			break;
		case GLFW_KEY_DELETE:
			setMode(Mode::NONE);
			for (auto& vertRef : selectedVertices)
				dynamic_cast<Mesh*>(vertRef.object)->deleteVertices(vertRef.vertexIndices);
			selectedVertices.clear();
			break;
		case GLFW_KEY_ENTER:
			if (mode == Mode::COMMAND_ENTER)
			{
				setMode(Mode::NONE);
				executeCommand();
			}
			else if (isMoveMode(mode))
				startPosition = selectedObject->getPosition();
			break;
		case GLFW_KEY_X:
			if (selectedObject != nullptr)
			{
				if (mode == Mode::MOVE_Y)
					setMode(Mode::MOVE_XY);
				else if (mode == Mode::MOVE_Z)
					setMode(Mode::MOVE_ZX);
				else
				{
					setMode(Mode::MOVE_X);
					startPosition = selectedObject->getPosition();
				}
				updateObjectTransform(getMousePos());
			}
			break;
		case GLFW_KEY_Y:
			if (selectedObject != nullptr)
			{
				if (mode == Mode::MOVE_Z)
					setMode(Mode::MOVE_YZ);
				else if (mode == Mode::MOVE_X)
					setMode(Mode::MOVE_XY);
				else
				{
					setMode(Mode::MOVE_Y);
					startPosition = selectedObject->getPosition();
				}
				updateObjectTransform(getMousePos());
			}
			break;
		case GLFW_KEY_Z:
			if (selectedObject != nullptr)
			{
				if (mode == Mode::MOVE_X)
					setMode(Mode::MOVE_ZX);
				else if (mode == Mode::MOVE_Y)
					setMode(Mode::MOVE_YZ);
				else
				{
					setMode(Mode::MOVE_Z);
					startPosition = selectedObject->getPosition();
				}
				updateObjectTransform(getMousePos());
			}
			break;
		}
	case Event::Type::MOUSE_PRESS:
		if (event.button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (!EditorDrawUtils::pickSelector(camera->castRay(screenToNDC(glm::ivec2(event.pos)))))
			{
				setMode(Mode::AREA_SELECT);
				areaSelect.start = glm::ivec2(event.pos);
				areaSelect.end = glm::ivec2(event.pos);
			}
		}
		break;
	case Event::Type::MOUSE_MOVE:
		if (mode == Mode::AREA_SELECT)
		{
			areaSelect.end = glm::ivec2(event.pos);
			auto nStart = screenToNDC(areaSelect.start);
			auto nEnd = screenToNDC(areaSelect.end);
			auto nnStart = glm::min(nStart, nEnd);
			auto nnEnd = glm::max(nStart, nEnd);
			selectedVertices = scene->selectVertices(nnStart, nnEnd - nnStart, camera->getProjViewMat());
		}
		else if (mode == Mode::MOVE_X || mode == Mode::MOVE_Y || mode == Mode::MOVE_Z ||
			mode == Mode::MOVE_XY || mode == Mode::MOVE_YZ || mode == Mode::MOVE_ZX)
			updateObjectTransform(event.pos);
		break;
	case Event::Type::MOUSE_RELEASE:
		if (event.button == GLFW_MOUSE_BUTTON_RIGHT && mode == Mode::AREA_SELECT)
		{
			setMode(Mode::NONE);
			areaSelect.end = glm::ivec2(event.pos);
			if (areaSelect.start == areaSelect.end)
			{
				auto objs = scene->selectObjects(camera->castRay(screenToNDC(areaSelect.start)));
				if (!objs.empty())
				{
					Object::ObjectRef::sort(objs);
					selectedObject = objs.front().object;
				}
				else
					selectedObject = nullptr;
			}
			else
				selectedObject = nullptr;
		}
		break;
	case Event::Type::CHAR:
		if (mode != Mode::COMMAND_ENTER && static_cast<char>(event.character) == ':')
		{
			setMode(Mode::COMMAND_ENTER);
			command = "";
		}
		if (mode == Mode::COMMAND_ENTER)
		{
			command += static_cast<char>(event.character);
			std::cout << static_cast<char>(event.character);
		}
		break;
	}
}

void Editor::update(float dt)
{
	camera->update(dt);
}

void Editor::render()
{
	scene->render(camera);

	for (const auto& vertRef : selectedVertices)
		EditorDrawUtils::drawVertices(dynamic_cast<Mesh*>(vertRef.object)->getVertices(vertRef.vertexIndices), camera->getProjViewMat());
	
	if (selectedObject != nullptr)
		EditorDrawUtils::drawSelector(dynamic_cast<Mesh*>(selectedObject)->getCenter(), camera);

	switch (mode)
	{
	case Editor::Mode::AREA_SELECT:
	{
		auto nStart = screenToNDC(areaSelect.start);
		auto nEnd = screenToNDC(areaSelect.end);
		auto nnStart = glm::min(nStart, nEnd);
		auto nnEnd = glm::max(nStart, nEnd);
		if (nStart != nEnd)
		EditorDrawUtils::drawSelection(nnStart, nnEnd - nnStart);
		break;
	}
	}
}

Editor& Editor::get(GLFWwindow* window)
{
	static Editor editor(window);

	return editor;
}

void Editor::setMode(Mode mode)
{
	if (this->mode == Mode::COMMAND_ENTER)
		std::cout << '\n';

	if (isMoveMode(this->mode))
		selectedObject->setPosition(startPosition);

	this->mode = mode;
}

Editor::Editor(GLFWwindow* window) : window(window)
{
	shader = Shader("shaders/vtest.glsl", "shaders/fcolor.glsl");

	scene = new Object();
	for (int i = 0; i < 10; i++)
	{
		Mesh* mesh = new Mesh(Mesh::cylinder(&shader));
		mesh->setPosition(glm::vec3(3 * i, 0, 0));
		scene->add(mesh);
		mesh = new Mesh(Mesh::cone(&shader));
		mesh->setPosition(glm::vec3(3 * i, 0, 2));
		scene->add(mesh);
		mesh = new Mesh(Mesh::cube(&shader));
		mesh->setScale(glm::vec3(static_cast<float>(i + 1) / 10.0f, 1.0f, 1.0f));
		mesh->setPosition(glm::vec3(3 * i, 0, 4));
		mesh->rotate(glm::vec3(1, 0, 0), i / 5.0f);
		scene->add(mesh);
	}

	//scene->rotate(glm::vec3(0, 1, 0), 1.5f);

	glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	camera = new MovingCamera(glm::vec3(0, 0, 2), static_cast<float>(windowSize.x) / windowSize.y);
	//dynamic_cast<MovingCamera*>(camera)->far = 1000.0f;
	//dynamic_cast<MovingCamera*>(camera)->movementSpeed = 0.1f;

	EditorDrawUtils::init();
}

Editor::~Editor()
{
	delete scene;
	delete camera;

	EditorDrawUtils::deinit();
}

void Editor::updateObjectTransform(const glm::dvec2& mousePos)
{
	auto ray = camera->castRay(screenToNDC(glm::ivec2(mousePos)));

	glm::vec3 newPosition;

	if (this->mode == Mode::MOVE_X || this->mode == Mode::MOVE_Y || this->mode == Mode::MOVE_Z)
	{
		glm::vec3 axis{};
		axis[static_cast<int>(mode) - static_cast<int>(Mode::MOVE_X)] = 1.0f;
		newPosition = startPosition + RayUtiles::projectLine(ray, startPosition, axis) * axis;
	}
	else
	{
		glm::vec3 u;
		glm::vec3 v;
		if (mode == Mode::MOVE_XY)
		{
			u = glm::vec3(1, 0, 0);
			v = glm::vec3(0, 1, 0);
		}
		else if (mode == Mode::MOVE_YZ)
		{
			u = glm::vec3(0, 1, 0);
			v = glm::vec3(0, 0, 1);
		}
		else
		{
			u = glm::vec3(0, 0, 1);
			v = glm::vec3(1, 0, 0);
		}

		const glm::vec2 uv = RayUtiles::projectPlane(ray, startPosition, u, v);
		
		newPosition = startPosition + uv.x * u + uv.y * v;
	}

	if (leftCtrlDown())
	{
		const auto delta = newPosition - startPosition;
		newPosition = startPosition + glm::round(delta);
	}

	selectedObject->setPosition(newPosition);
}

void Editor::executeCommand()
{
	std::stringstream sss;
	sss << command;
	sss.ignore();
	char token{};

	auto getAxis = [](char token, glm::vec3& axis) {
		switch (token)
		{
		case 'x':
			axis = glm::vec3(1, 0, 0);
			break;
		case 'y':
			axis = glm::vec3(0, 1, 0);
			break;
		case 'z':
			axis = glm::vec3(0, 0, 1);
			break;
		default:
			return false;
			break;
		}

		return true;
	};

	auto getFloat = [&](float& f) {
		sss >> f;
		return !sss.fail();
	};

	auto next = [&]() {
		sss >> token;
		return !sss.fail();
	};


	if (!next()) return;

	glm::vec3 axis;
	if (getAxis(token, axis) && selectedObject != nullptr)
	{
		float off;
		if (getFloat(off))
		{
			const auto& pos = selectedObject->getPosition();
			selectedObject->setPosition(pos + axis * off);
		}
	}
	else if (token == 's')
	{
		if (!next())
		{
			if (selectedObject != nullptr)
			{
				const auto& scale = selectedObject->getScale();
				std::cout << scale.x << ", " << scale.y << ", " << scale.z << '\n';
			}
		}
		else if (getAxis(token, axis) && selectedObject != nullptr)
		{
			float scale;
			if (getFloat(scale))
			{
				const auto& initialScale = selectedObject->getScale();
				selectedObject->setScale((glm::vec3(1) - axis) * initialScale + axis * scale);
			}
		}
	}
	else if (token == 'r')
	{
		if (next() && selectedObject != nullptr)
		{
			if (getAxis(token, axis))
			{
				float rotation;
				if (getFloat(rotation))
				{
					selectedObject->rotate(axis, glm::radians(rotation));
				}
			}
		}
	}
}

glm::dvec2 Editor::getMousePos()
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return glm::dvec2(x, y);
}

bool Editor::isMoveMode(Mode mode) const
{
	return mode == Mode::MOVE_X || mode == Mode::MOVE_Y || mode == Mode::MOVE_Z ||
		mode == Mode::MOVE_XY || mode == Mode::MOVE_YZ || mode == Mode::MOVE_ZX;
}

bool Editor::leftCtrlDown() const
{
	return glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
}

bool Editor::leftShiftDown() const
{
	return glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
}

glm::vec2 Editor::screenToNDC(const glm::ivec2& v) const
{
	glm::vec2 nv = glm::vec2(v) / glm::vec2(windowSize);
	return glm::vec2((nv.x - 0.5f) * 2.0f, (0.5f - nv.y) * 2.0f);
}
