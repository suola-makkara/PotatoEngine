#include "ray_utiles.hpp"

float RayUtiles::projectLine(const Ray& ray, const glm::vec3& origin, const glm::vec3& dir)
{
	const glm::vec3 d = origin - ray.origin;
	const float A = glm::dot(d, dir);
	const float B = glm::dot(d, ray.direction);
	const float C = glm::dot(dir, ray.direction);

	return (B * C - A) / (1.0f - C * C);
}

glm::vec2 RayUtiles::projectPlane(const Ray& ray, const glm::vec3& origin, const glm::vec3& u, const glm::vec3& v)
{
	glm::vec3 n = glm::cross(u, v);

	const glm::vec3 d = origin - ray.origin;
	const float A = 1.0f / glm::dot(ray.direction, n);

	return glm::vec2(glm::dot(d, A * glm::dot(ray.direction, u) * n - u), glm::dot(d, A * glm::dot(ray.direction, v) * n - v));
}