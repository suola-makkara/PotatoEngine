#include "editor.hpp"
#include "mesh.hpp"
#include "moving_camera.hpp"
#include "editor_draw_utils.hpp"
#include "defs.hpp"
#include "ray.hpp"
#include "ray_cast.hpp"

#include <algorithm>

void Editor::handleEvent(const Event& event)
{
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
			mode = Mode::NONE;
			selectedVertices.clear();
			break;
		case GLFW_KEY_DELETE:
			mode = Mode::NONE;
			for (auto vertRef : selectedVertices)
				dynamic_cast<Mesh*>(vertRef.object)->deleteVertices(vertRef.vertexIndices);
			selectedVertices.clear();
			break;
		}
	case Event::Type::MOUSE_PRESS:
		if (event.button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			mode = Mode::AREA_SELECT;
			areaSelect.start = glm::ivec2(event.pos);
			areaSelect.end = glm::ivec2(event.pos);
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
		break;
	case Event::Type::MOUSE_RELEASE:
		if (event.button == GLFW_MOUSE_BUTTON_RIGHT && mode == Mode::AREA_SELECT)
		{
			mode = Mode::NONE;
			areaSelect.end = glm::ivec2(event.pos);
			if (areaSelect.start == areaSelect.end)
			{
				MovingCamera* mCamera = dynamic_cast<MovingCamera*>(camera);
				auto dir = RayCast::castCameraRay(mCamera->fov, mCamera->getAspect(), screenToNDC(areaSelect.start));
				Ray ray;
				ray.direction = glm::transpose(glm::mat3(camera->getViewMat())) * dir;
				ray.origin = camera->getPosition();

				auto objs = scene->selectObjects(ray);
				if (!objs.empty())
				{
					objs.sort([](const Object::ObjectRef& ref0, const Object::ObjectRef& ref1) { return ref0.dist < ref1.dist; });
					selectedObject = objs.front().object;
				}
				else
					selectedObject = nullptr;
			}
			else
				selectedObject = nullptr;
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

Editor::Editor(GLFWwindow* window)
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

	scene->rotate(glm::vec3(0, 1, 0), 1.5f);

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

glm::vec2 Editor::screenToNDC(const glm::ivec2& v)
{
	glm::vec2 nv = glm::vec2(v) / glm::vec2(windowSize);
	return glm::vec2((nv.x - 0.5f) * 2.0f, (0.5f - nv.y) * 2.0f);
}
