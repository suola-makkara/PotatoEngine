cmake_minimum_required (VERSION 3.8)

set (LIB
	glfw
	glm
	glad
)

set (CPP
	main.cpp
	shader.cpp
	event_registry.cpp
)

set (HPP
	shader.hpp
	camera.hpp
	event.hpp
	event_registry.hpp
)

add_executable (${PROJECT_NAME} ${CPP} ${HPP})
target_link_libraries(${PROJECT_NAME} ${LIB})