#pragma once

#include "ray.hpp"

class IntersectionTest
{
public:
	static float intersect(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
};