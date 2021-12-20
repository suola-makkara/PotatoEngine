cmake_minimum_required (VERSION 3.8)

set (LIB
	glfw
	glm
	glad
)

set (CPP
	main.cpp
)

set (HPP
	
)

add_executable (${PROJECT_NAME} ${CPP} ${HPP})
target_link_libraries(${PROJECT_NAME} ${LIB})