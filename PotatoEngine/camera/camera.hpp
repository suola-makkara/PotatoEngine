#pragma once

#include "event.hpp"

#include "glm\glm.hpp"

class Camera
{
public:
	virtual ~Camera() = default;

	virtual glm::vec3 getDirection() const = 0;

	virtual glm::vec3 getPosition() const = 0;

	virtual glm::mat4 getViewMat() const = 0;

	virtual glm::mat4 getProjMat() const = 0;

	virtual void update(float dt) = 0;

	glm::mat4 getProjViewMat() const { return getProjMat() * getViewMat(); }

	virtual void handleEvent(const Event& e) = 0;
};