cmake_minimum_required (VERSION 3.8)
project ("PotatoEngine")

set(GLAD_ALL_EXTENSIONS ON)

add_subdirectory("ext/glad")
add_subdirectory("ext/glfw")
add_subdirectory("ext/glm")

add_subdirectory (${PROJECT_NAME})
