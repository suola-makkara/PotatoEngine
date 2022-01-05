#pragma once

#include "ray.hpp"

class RayCast
{
public:
	static glm::vec3 castCameraRay(float fov, float aspect, const glm::vec2& coords);
};