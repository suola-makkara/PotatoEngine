#include "GLFW\glfw3.h"
#include "glm\vec3.hpp"
#include "glad\glad.h"

#include <iostream>

int main()
{
	glm::vec3 vec(1, 2, 3);

	std::cout << vec.x << '\n';

	return 0;
}