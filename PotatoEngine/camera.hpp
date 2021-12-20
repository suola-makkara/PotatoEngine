#pragma once

#include "glm\glm.hpp"

class Camera
{
public:
	virtual ~Camera() = default;

	virtual glm::mat4 getViewMat() const;

	virtual glm::mat4 getProjMat() const;

	glm::mat4 getProjViewMat() const { return getProjMat() * getViewMat(); }

	virtual void handleEvent()
};