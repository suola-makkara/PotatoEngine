#include "editor.hpp"
#include "mesh.hpp"
#include "moving_camera.hpp"
#include "editor_draw_utils.hpp"
#include "defs.hpp"
#include "ray.hpp"
#include "ray_cast.hpp"
#include "ray_utiles.hpp"
#include "command_parser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>

// TODO
/*	
*** editor core ***
* - separate local and global transforms
* - key mappings to commands with config file
* - selecting multiple objects / verts
* - command stack for undo / redo
* - orthographic camera views
* - reference grid
* -
* 
*** object editor ***
* - Object deletion
* -
* 
*** vertex editor ***
* - selecting face / edges
* - single vertex selection
* - pass through selection toggle
* - moving vertices
* - face subdivision
* - vertex snapping / edge collapsing
* -
* 
*** rendering ***
* - lights
* -
* 
*/

Editor::Editor(GLFWwindow* window) : window(window), drawUtils(std::make_unique<EditorDrawUtils>())
{
	shader = Shader("shaders/vbasic.glsl", "shaders/gnormals.glsl", "shaders/fbasic.glsl");
	wireframeShader = Shader("shaders/vtest.glsl", "shaders/fcolor.glsl");

	context.scene = std::make_unique<Object>();
	for (int i = 0; i < 10; i++)
	{
		auto mesh = Mesh::cylinder(&shader, &wireframeShader);
		mesh->setPosition(glm::vec3(3 * i, 0, 0));
		context.scene->add(std::move(mesh));
		mesh = Mesh::cone(&shader, &wireframeShader);
		mesh->setPosition(glm::vec3(3 * i, 0, 2));
		context.scene->add(std::move(mesh));
		mesh = Mesh::cube(&shader, &wireframeShader);
		mesh->setScale(glm::vec3(static_cast<float>(i + 1) / 10.0f, 1.0f, 1.0f));
		mesh->setPosition(glm::vec3(3 * i, 0, 4));
		mesh->rotate(glm::vec3(1, 0, 0), i / 5.0f);
		context.scene->add(std::move(mesh));
	}

	glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	camera = std::make_unique<MovingCamera>(glm::vec3(0, 0, 2), static_cast<float>(windowSize.x) / windowSize.y);
}

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
/*		case GLFW_KEY_0:
			submitCommand("test");
			break;
		case GLFW_KEY_RIGHT:
			submitCommand("redo");
			break;
		case GLFW_KEY_LEFT:
			submitCommand("revert");
			break;
		case GLFW_KEY_1:
			submitCommand("asdwadw");
			break;
		case GLFW_KEY_ESCAPE:
			setMode(Mode::NONE);
			if (editMode == EditMode::VERTEX)
				selectedVertices.clear();
			break;
		case GLFW_KEY_DELETE:
			setMode(Mode::NONE);
			if (editMode == EditMode::VERTEX)
			{
				for (auto& vertRef : selectedVertices)
					dynamic_cast<Mesh*>(vertRef.object)->deleteVertices(vertRef.vertexIndices);
				selectedVertices.clear();
			}
			break;*/
		case GLFW_KEY_ENTER:
			if (mode == Mode::COMMAND_ENTER)
			{
				//setMode(Mode::NONE);
				//executeCommand();
				mode = Mode::NONE;
				std::cout << '\n';
				submitCommand(command.substr(1));
			}
			//else if (editMode == EditMode::OBJECT && isMoveMode(mode))
			//	startPosition = selectedObject->getPosition();
			break;
		/*case GLFW_KEY_O:
			setEditMode(editMode == EditMode::OBJECT ? EditMode::VERTEX : EditMode::OBJECT);
		case GLFW_KEY_M:
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
				scene->add(selectedObject->copy());
			break;
		case GLFW_KEY_H:
			scene->setRenderMode(scene->getRenderMode() == Object::RenderMode::DEFAULT ?
				Object::RenderMode::WIRE_FRAME : Object::RenderMode::DEFAULT);
			break;
		case GLFW_KEY_R:
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
			{
				setMode(Mode::ROTATE);
				startBasis = selectedObject->getBasis();
			}
			break;
		case GLFW_KEY_T:
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
			{
				setMode(Mode::SCALE);
				startScale = selectedObject->getScale();
			}
			break;
		case GLFW_KEY_X:
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
			{
				if (mode == Mode::ROTATE)
					setMode(Mode::ROTATE_X);
				else if (mode == Mode::SCALE)
					setMode(Mode::SCALE_X);
				else if (mode == Mode::SCALE_Y)
					setMode(Mode::SCALE_XY);
				else if (mode == Mode::SCALE_Z)
					setMode(Mode::SCALE_ZX);
				else if (mode == Mode::MOVE_Y)
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
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
			{
				if (mode == Mode::ROTATE)
					setMode(Mode::ROTATE_Y);
				else if (mode == Mode::SCALE)
					setMode(Mode::SCALE_Y);
				else if (mode == Mode::SCALE_X)
					setMode(Mode::SCALE_XY);
				else if (mode == Mode::SCALE_Z)
					setMode(Mode::SCALE_YZ);
				else if (mode == Mode::MOVE_Z)
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
			if (editMode == EditMode::OBJECT && selectedObject != nullptr)
			{
				if (mode == Mode::ROTATE)
					setMode(Mode::ROTATE_Z);
				else if (mode == Mode::SCALE)
					setMode(Mode::SCALE_Z);
				else if (mode == Mode::SCALE_X)
					setMode(Mode::SCALE_ZX);
				else if (mode == Mode::SCALE_Y)
					setMode(Mode::SCALE_YZ);
				else if (mode == Mode::MOVE_X)
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
			break;*/
		}
		break;
	//case Event::Type::MOUSE_PRESS:
	//	if (event.button == GLFW_MOUSE_BUTTON_RIGHT)
	//	{
	//		//if (!EditorDrawUtils::pickSelector(camera->castRay(screenToNDC(glm::ivec2(event.pos))))) { }

	//		setMode(Mode::AREA_SELECT);
	//		areaSelect.start = glm::ivec2(event.pos);
	//		areaSelect.end = glm::ivec2(event.pos);
	//	}
	//	break;
	//case Event::Type::MOUSE_MOVE:
	//	if (mode == Mode::AREA_SELECT)
	//	{
	//		areaSelect.end = glm::ivec2(event.pos);

	//		if (editMode == EditMode::VERTEX)
	//		{
	//			auto nStart = screenToNDC(areaSelect.start);
	//			auto nEnd = screenToNDC(areaSelect.end);
	//			auto nnStart = glm::min(nStart, nEnd);
	//			auto nnEnd = glm::max(nStart, nEnd);
	//			selectedVertices = scene->selectVertices(nnStart, nnEnd - nnStart, camera->getProjViewMat());
	//		}
	//	}
	//	else if (editMode == EditMode::OBJECT && (isMoveMode(mode) || isRotationMode(mode) || isScaleMode(mode)))
	//		updateObjectTransform(event.pos);
	//	break;
	//case Event::Type::MOUSE_RELEASE:
	//	if (event.button == GLFW_MOUSE_BUTTON_RIGHT && mode == Mode::AREA_SELECT)
	//	{
	//		setMode(Mode::NONE);
	//		areaSelect.end = glm::ivec2(event.pos);

	//		if (editMode == EditMode::OBJECT)
	//		{
	//			if (areaSelect.start == areaSelect.end)
	//			{
	//				auto objs = scene->selectObjects(camera->castRay(screenToNDC(areaSelect.start)));
	//				if (!objs.empty())
	//				{
	//					Object::ObjectRef::sort(objs);
	//					if (objs.front().object == selectedObject)
	//						unselectObject();
	//					else
	//						selectObject(objs.front().object);
	//				}
	//				else
	//					unselectObject();
	//			}
	//			else
	//				unselectObject();
	//		}
	//	}
	//	break;
	case Event::Type::CHAR:
		if (mode != Mode::COMMAND_ENTER && static_cast<char>(event.character) == ':')
		{
			//setMode(Mode::COMMAND_ENTER);
			mode = Mode::COMMAND_ENTER;
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
	context.scene->render(camera.get());

	//for (const auto& vertRef : selectedVertices)
	//	EditorDrawUtils::drawVertices(dynamic_cast<Mesh*>(vertRef.object)->getVertices(vertRef.vertexIndices), camera->getProjViewMat());
	
	//switch (mode)
	//{
	//case Editor::Mode::AREA_SELECT:
	//{
	//	auto nStart = screenToNDC(areaSelect.start);
	//	auto nEnd = screenToNDC(areaSelect.end);
	//	auto nnStart = glm::min(nStart, nEnd);
	//	auto nnEnd = glm::max(nStart, nEnd);
	//	if (nStart != nEnd)
	//	EditorDrawUtils::drawSelection(nnStart, nnEnd - nnStart);
	//	break;
	//}
	//}
}

void Editor::submitCommand(const std::string& commandString)
{
	auto commands = CommandParser::parseCommand(commandString);
	for (auto& command : commands)
	{
		if (command->execute(context))
		{
			context.revertStack.push(std::move(command));
			context.redoStack = std::stack<std::unique_ptr<Command>>();
		}
	}
}

//void Editor::setMode(Mode mode)
//{
//	if (this->mode == Mode::COMMAND_ENTER)
//		std::cout << '\n';
//
//	if (isMoveMode(this->mode) && mode != Mode::AREA_SELECT)
//		selectedObject->setPosition(startPosition);
//	else if (isRotationMode(this->mode) && mode != Mode::AREA_SELECT)
//		selectedObject->setBasis(startBasis);
//	else if (isScaleMode(this->mode) && mode != Mode::AREA_SELECT)
//		selectedObject->setScale(startScale);
//
//	this->mode = mode;
//}

//void Editor::setEditMode(EditMode mode)
//{
//	setMode(Mode::NONE);
//
//	if (editMode == EditMode::OBJECT)
//		unselectObject();
//	else if (editMode == EditMode::VERTEX)
//		selectedVertices.clear();
//
//	editMode = mode;
//}

//void Editor::selectObject(Object* object)
//{
//	if (selectedObject != nullptr)
//		dynamic_cast<Mesh*>(selectedObject)->color = Mesh::BASE_COLOR;
//
//	selectedObject = object;
//	if (selectedObject != nullptr)
//		dynamic_cast<Mesh*>(selectedObject)->color = Mesh::SELECTED_COLOR;
//}
//
//void Editor::unselectObject()
//{
//	selectObject(nullptr);
//}





/*void Editor::updateObjectTransform(const glm::dvec2& mousePos)
{
	auto ray = camera->castRay(screenToNDC(glm::ivec2(mousePos)));
	
	if (isRotationMode(mode))
	{
		glm::vec3 axis{};
		axis[static_cast<int>(mode) - static_cast<int>(Mode::ROTATE_X)] = 1.0f;

		glm::vec3 u;
		glm::vec3 v;
		if (mode == Mode::ROTATE_X)
		{
			u = glm::vec3(0, 1, 0);
			v = glm::vec3(0, 0, 1);
		}
		else if (mode == Mode::ROTATE_Y)
		{
			u = glm::vec3(0, 0, 1);
			v = glm::vec3(1, 0, 0);
		}
		else
		{
			u = glm::vec3(1, 0, 0);
			v = glm::vec3(0, 1, 0);
		}

		const glm::vec2 uv = RayUtiles::projectPlane(ray, selectedObject->getPosition(), u, v);

		selectedObject->setBasis(startBasis);
		selectedObject->rotate(axis, std::atan2f(uv.y, uv.x));
	}
	else if (isScaleMode(mode))
	{
		glm::vec3 newScale;

		const glm::vec3& pos = selectedObject->getPosition();

		if (mode == Mode::SCALE_X || mode == Mode::SCALE_Y || mode == Mode::SCALE_Z)
		{
			glm::vec3 axis{};
			axis[static_cast<int>(mode) - static_cast<int>(Mode::SCALE_X)] = 1.0f;
			newScale = startScale * (1.0f - axis) + RayUtiles::projectLine(ray, pos, axis) * axis;
		}
		else
		{
			glm::vec3 u;
			glm::vec3 v;
			if (mode == Mode::SCALE_XY)
			{
				u = glm::vec3(1, 0, 0);
				v = glm::vec3(0, 1, 0);
			}
			else if (mode == Mode::SCALE_YZ)
			{
				u = glm::vec3(0, 1, 0);
				v = glm::vec3(0, 0, 1);
			}
			else
			{
				u = glm::vec3(0, 0, 1);
				v = glm::vec3(1, 0, 0);
			}

			const glm::vec2 uv = RayUtiles::projectPlane(ray, pos, u, v);
			newScale = startScale * (1.0f - u - v) + uv.x * u + uv.y * v;
		}

		if (leftCtrlDown())
		{
			const auto delta = newScale - startScale;
			newScale = startScale + glm::round(delta);
		}

		selectedObject->setScale(glm::abs(newScale));
	}
	else
	{
		glm::vec3 newPosition;

		if (mode == Mode::MOVE_X || mode == Mode::MOVE_Y || mode == Mode::MOVE_Z)
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
}*/

/*void Editor::executeCommand()
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
}*/

//glm::dvec2 Editor::getMousePos()
//{
//	double x, y;
//	glfwGetCursorPos(window, &x, &y);
//	return glm::dvec2(x, y);
//}

/*bool Editor::isMoveMode(Mode mode) const
{
	return mode == Mode::MOVE_X || mode == Mode::MOVE_Y || mode == Mode::MOVE_Z ||
		mode == Mode::MOVE_XY || mode == Mode::MOVE_YZ || mode == Mode::MOVE_ZX;
}

bool Editor::isRotationMode(Mode mode) const
{
	return mode == Mode::ROTATE_X || mode == Mode::ROTATE_Y || mode == Mode::ROTATE_Z;
}

bool Editor::isScaleMode(Mode mode) const
{
	return mode == Mode::SCALE_X || mode == Mode::SCALE_Y || mode == Mode::SCALE_Z ||
		mode == Mode::SCALE_XY || mode == Mode::SCALE_YZ || mode == Mode::SCALE_ZX;
}

bool Editor::leftCtrlDown() const
{
	return glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
}

bool Editor::leftShiftDown() const
{
	return glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
}*/

//glm::vec2 Editor::screenToNDC(const glm::ivec2& v) const
//{
//	glm::vec2 nv = glm::vec2(v) / glm::vec2(windowSize);
//	return glm::vec2((nv.x - 0.5f) * 2.0f, (0.5f - nv.y) * 2.0f);
//}
