#include "intersection_test.hpp"


#define CROSS(r, a, b) const float r ## x = a ## y * b ## z - a ## z * b ## y, r ## y = a ## z * b ## x - a ## x * b ## z, r ## z = a ## x * b ## y - a ## y * b ## x
#define DOT(a, b) (a ## x * b ## x + a ## y * b ## y + a ## z * b ## z)

float IntersectionTest::intersect(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
	static const float EPSILON = 0.000001f;

	const float x0 = v0.x, y0 = v0.y, z0 = v0.z;
	const float x1 = v1.x, y1 = v1.y, z1 = v1.z;
	const float x2 = v2.x, y2 = v2.y, z2 = v2.z;

	const float dx = ray.direction.x, dy = ray.direction.y, dz = ray.direction.z;

	const float e1x = x1 - x0, e1y = y1 - y0, e1z = z1 - z0;
	const float e2x = x2 - x0, e2y = y2 - y0, e2z = z2 - z0;

	CROSS(h, d, e2);
	const float q = DOT(e1, h);
	if (q > -EPSILON && q < EPSILON) return FLT_MAX;

	const float iq = 1.0 / q;
	const float Dx = ray.origin.x - x0, Dy = ray.origin.y - y0, Dz = ray.origin.z - z0;
	const float u = iq * DOT(D, h);
	if (u < 0.0 || u > 1.0) return FLT_MAX;

	CROSS(i, D, e1);
	const float v = iq * DOT(d, i);
	if (v < 0.0 || u + v > 1.0) return FLT_MAX;

	return iq * DOT(e2, i);

	//if (t > tmin && t < h.t)
	//{
	//	CROSS(n, e1, e2);
	//	h.set(t, material_, FW::Vec3f(nx, ny, nz).normalized());
	//	return true;
	//}
	//else
	//	return false;
}
