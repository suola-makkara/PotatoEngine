#include "moving_camera.hpp"

#include "GLFW\glfw3.h"
#include "glm\gtc\constants.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include <iostream>

MovingCamera::MovingCamera(const glm::vec3& pos, float aspect)
	: pos(pos), aspect(aspect)
{

}

glm::vec3 MovingCamera::getDirection() const
{
	float sinPitch = glm::sin(pitch);
	float cosPitch = glm::cos(pitch);
	float sinYaw = glm::sin(yaw);
	float cosYaw = glm::cos(yaw);

	return glm::vec3(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
}

glm::vec3 MovingCamera::getPosition() const
{
	return pos;
}

glm::mat4 MovingCamera::getViewMat() const
{
	glm::vec3 back = -getDirection();
	glm::vec3 right = glm::normalize(glm::cross(up, back));
	glm::vec3 upOrth = glm::cross(back, right);

	glm::mat4 view;
	view[0] = glm::vec4(right, 0);
	view[1] = glm::vec4(upOrth, 0);
	view[2] = glm::vec4(back, 0);
	view[3] = glm::vec4(0, 0, 0, 1);

	return glm::transpose(view) * glm::translate(glm::identity<glm::mat4>(), -pos);;
}

glm::mat4 MovingCamera::getProjMat() const
{
	return glm::perspective(glm::radians(fov), aspect, near, far);
}

void MovingCamera::update(float dt)
{
	auto dir = getDirection();
	auto right = glm::normalize(glm::cross(up, -dir));

	pos += movementSpeed
		* (static_cast<float>(movement.x) * right
		+ static_cast<float>(movement.y) * up
		+ static_cast<float>(movement.z) * dir);
}

void MovingCamera::handleEvent(const Event& e)
{
	switch (e.type)
	{
	case Event::Type::KEY_PRESS:
		switch (e.key)
		{
		case GLFW_KEY_W:
			movement.z += 1;
			break;
		case GLFW_KEY_S:
			movement.z -= 1;
			break;
		case GLFW_KEY_A:
			movement.x -= 1;
			break;
		case GLFW_KEY_D:
			movement.x += 1;
			break;
		/*case GLFW_KEY_SPACE:
			movement.y += 1;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			movement.y -= 1;
			break;*/
		}
		break;
	case Event::Type::KEY_RELEASE:
		switch (e.key)
		{
		case GLFW_KEY_W:
			movement.z -= 1;
			break;
		case GLFW_KEY_S:
			movement.z += 1;
			break;
		case GLFW_KEY_A:
			movement.x += 1;
			break;
		case GLFW_KEY_D:
			movement.x -= 1;
			break;
		/*case GLFW_KEY_SPACE:
			movement.y -= 1;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			movement.y += 1;
			break;*/
		}
		break;
	case Event::Type::MOUSE_PRESS:
		if (e.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			mouseDown = true;
			prevMousePos = static_cast<glm::vec2>(e.pos);
		}
		break;
	case Event::Type::MOUSE_RELEASE:
		if (e.button == GLFW_MOUSE_BUTTON_LEFT)
			mouseDown = false;
		break;
	case Event::Type::MOUSE_MOVE:
		if (mouseDown)
		{
			glm::vec2 mousePos = static_cast<glm::vec2>(e.pos);

			glm::vec2 deltaMouse = mousePos - prevMousePos;

			yaw += deltaMouse.x * mouseSensitiviy;
			pitch -= deltaMouse.y * mouseSensitiviy;
			if (pitch > glm::half_pi<float>() - glm::epsilon<float>()) pitch = glm::half_pi<float>() - glm::epsilon<float>();
			else if (pitch < -glm::half_pi<float>() + glm::epsilon<float>()) pitch = -glm::half_pi<float>() + glm::epsilon<float>();

			prevMousePos = mousePos;
		}
		break;
	case Event::Type::FRAME_BUFFER_RESIZE:
		aspect = static_cast<float>(e.size.x) / e.size.y;
		break;
	}
}

float MovingCamera::getAspect() const
{
	return aspect;
}

const glm::vec3 MovingCamera::up = glm::vec3(0, 1, 0);