#pragma once

#include "glm/glm.hpp"

class Simplex
{
public:
	static float simplex2D(const glm::vec2& p, int seed);

	static void init();

private:
	static const int GRADS_2D = 83;
	static glm::vec2 grad2D[GRADS_2D];
};