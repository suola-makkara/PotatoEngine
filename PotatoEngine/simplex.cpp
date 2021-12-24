#include "simplex.hpp"

#include "glm/gtc/constants.hpp"

#include <vector>

float Simplex::simplex2D(const glm::vec2& ps, int seed)
{
	static const float GS = (1.0f - glm::sqrt(3.0f)) / (2.0f * glm::sqrt(3.0f));
	static const float SG = (glm::sqrt(3.0f) - 1.0f) / 2.0f;
	static const float C = 81.0f * glm::sqrt(3.0f) / glm::sqrt(2.0f);

	const glm::vec2 pg = ps + SG * (ps.x + ps.y);
	const glm::ivec2 g0 = glm::ivec2(glm::floor(pg));
	const glm::vec2 g0s = glm::vec2(g0) + GS * (g0.x + g0.y);
	const glm::vec2 pf0 = ps - g0s;

	const glm::ivec2 g1off = pf0.x > pf0.y ? glm::ivec2(1, 0) : glm::ivec2(0, 1);
	
	const glm::ivec2 g1 = g0 + g1off;
	const glm::ivec2 g2 = g0 + glm::ivec2(1, 1);
	
	const glm::vec2 g1s = glm::vec2(g1) + GS * (g1.x + g1.y);
	const glm::vec2 g2s = glm::vec2(g2) + GS * (g2.x + g2.y);
	
	const glm::vec2 pf1 = ps - g1s;
	const glm::vec2 pf2 = ps - g2s;

	glm::vec3 d = glm::max(glm::vec3(0.0f), glm::vec3(0.5f) - glm::vec3(glm::dot(pf0, pf0), glm::dot(pf1, pf1), glm::dot(pf2, pf2)));
	d *= d;
	d *= d;

	glm::vec3 g;
	const int i0 = static_cast<unsigned int>(std::hash<int>{}(g0.x) ^ (std::hash<int>{}(g0.y) << 1) ^ seed) % GRADS_2D;
	const int i1 = static_cast<unsigned int>(std::hash<int>{}(g1.x) ^ (std::hash<int>{}(g1.y) << 1) ^ seed) % GRADS_2D;
	const int i2 = static_cast<unsigned int>(std::hash<int>{}(g2.x) ^ (std::hash<int>{}(g2.y) << 1) ^ seed) % GRADS_2D;
	g.x = glm::dot(pf0, grad2D[i0]);
	g.y = glm::dot(pf1, grad2D[i1]);
	g.z = glm::dot(pf2, grad2D[i2]);

	return C * glm::dot(d, g);
}

void Simplex::init()
{
	for (int i = 0; i < GRADS_2D; i++)
	{
		const float a = glm::two_pi<float>() * i / GRADS_2D;
		grad2D[i] = glm::normalize(glm::vec2(glm::sin(a), glm::cos(a)));
	}
}

glm::vec2 Simplex::grad2D[]{};