#pragma once

#include "ray.hpp"

class RayUtiles
{
public:
	static float projectLine(const Ray& ray, const glm::vec3& origin, const glm::vec3& dir);

	static glm::vec2 projectPlane(const Ray& ray, const glm::vec3& origin, const glm::vec3& u, const glm::vec3& v);
};