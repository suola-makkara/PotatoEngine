cmake_minimum_required (VERSION 3.12)
project ("PotatoEngine")

set(GLAD_ALL_EXTENSIONS ON)

add_subdirectory("ext/glad")
add_subdirectory("ext/glfw")
add_subdirectory("ext/glm")

add_custom_target(test_target ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/shaders"
	COMMENT "Creating symlink"
)

add_subdirectory (${PROJECT_NAME})