cmake_minimum_required (VERSION 3.8)

set (LIB
	glfw
	glm
	glad
)

set (CPP
	main.cpp
	shader.cpp
)

set (HPP
	shader.hpp
)

add_executable (${PROJECT_NAME} ${CPP} ${HPP} "shader.cpp" "shader.hpp")
target_link_libraries(${PROJECT_NAME} ${LIB})