#pragma once

#include "glm/glm.hpp"

#include <vector>

class Simplex
{
public:
	static float simplex2D(const glm::vec2& ps, int seed);

	static std::vector<float> simplex2Dgrid(const glm::vec2& ps, float step, int steps, int seed);

	static void init();

private:
	static const int GRADS_2D = 83;
	static glm::vec2 grad2D[GRADS_2D];

	static bool initialized;
};