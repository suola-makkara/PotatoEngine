cmake_minimum_required (VERSION 3.15)
project ("PotatoEngine")

set(GLAD_ALL_EXTENSIONS ON)

add_subdirectory("ext/glad")
add_subdirectory("ext/glfw")
add_subdirectory("ext/glm")

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory("ext/gtest")

add_custom_target(test_target ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/shaders"
	COMMENT "Creating symlink"
)

add_subdirectory (${PROJECT_NAME})

# --- testing ---
option(POTATO_ENGINE_BUILD_TESTS "Potato engine build tests" ON)

if (POTATO_ENGINE_BUILD_TESTS)
	enable_testing()
	add_subdirectory ("tests")
endif()