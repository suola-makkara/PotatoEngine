#include "ray_cast.hpp"

glm::vec3 RayCast::castCameraRay(float fov, float aspect, const glm::vec2& coords)
{
	float y = glm::tan(glm::radians(fov / 2.0f));
	float x = aspect * y;

	glm::vec3 v(coords.x * x, coords.y * y, -1.0f);
	return glm::normalize(v);
}