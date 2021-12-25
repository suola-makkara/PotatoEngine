#define GLM_SWIZZLE
#include "simplex.hpp"

#include "GLFW\glfw3.h"
#include "glm/gtc/constants.hpp"
//#include "glm/gtc/swizzle.hpp"

#include <iostream>
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

std::vector<float> Simplex::simplex2Dgrid(const glm::vec2& ps, float step, int steps, int seed)
{
	static const float GS = (1.0f - glm::sqrt(3.0f)) / (2.0f * glm::sqrt(3.0f));
	static const float SG = (glm::sqrt(3.0f) - 1.0f) / 2.0f;
	static const float C = 81.0f * glm::sqrt(3.0f) / glm::sqrt(2.0f);

	const glm::vec2 pse = ps + step * steps;

	const glm::vec2 pg = ps + SG * (ps.x + ps.y);
	const glm::vec2 pge = pse + SG * (pse.x + pse.y);

	const glm::ivec2 g0 = glm::ivec2(glm::floor(pg));
	const glm::ivec2 g1 = glm::ivec2(glm::ceil(pge));

	std::vector<std::vector<std::pair<glm::vec2, glm::vec2>>> preg;
	preg.reserve(g1.x - g0.x + 1);

	auto t0 = glfwGetTime();
	for (int x = g0.x; x <= g1.x; x++)
	{
		preg.push_back(std::vector<std::pair<glm::vec2, glm::vec2>>());
		preg.back().reserve(g1.y - g0.y + 1);
		for (int y = g0.y; y <= g1.y; y++)
		{
			const glm::vec2 gs = glm::vec2(x, y) + GS * (x + y);

			const int i = static_cast<unsigned int>(std::hash<int>{}(x) ^ (std::hash<int>{}(y) << 1) ^ seed) % GRADS_2D;
			const glm::vec2 grad = grad2D[i];

			preg.back().push_back(std::make_pair(gs, grad));
		}
	}

	std::vector<float> noise;
	noise.reserve((steps + 1) * (steps + 1));
	t0 = glfwGetTime();
	for (int y = 0; y <= steps; y++)
		for (int x = 0; x <= steps; x++)
		{
			const glm::vec2 pss = ps + glm::vec2(x, y) * step;

			const glm::ivec2 g0 = glm::ivec2(glm::floor(pss + SG * (pss.x + pss.y)));
			const auto& g0ss = preg[g0.x][g0.y];

			const glm::vec2 pf0 = pss - g0ss.first;
			
			const auto& g1ss = pf0.x > pf0.y ? preg[g0.x + 1][g0.y] : preg[g0.x][g0.y + 1];
			const auto& g2ss = preg[g0.x + 1][g0.y + 1];

			const glm::vec2 pf1 = pss - g1ss.first;
			const glm::vec2 pf2 = pss - g2ss.first;

			glm::vec3 d = glm::max(glm::vec3(0.0f), glm::vec3(0.5f)
				- glm::vec3(glm::dot(pf0, pf0), glm::dot(pf1, pf1), glm::dot(pf2, pf2)));
			d *= d;
			d *= d;

			const glm::vec3 gg = glm::vec3(glm::dot(pf0, g0ss.second),
				glm::dot(pf1, g1ss.second), glm::dot(pf2, g2ss.second));

			noise.push_back(C * glm::dot(d, gg));
		}

	return noise;
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