cmake_minimum_required (VERSION 3.12)

set (LIB
	glfw
	glm
	glad
)

file (GLOB_RECURSE CPP CONFIGURE_DEPENDS "*.cpp")
file (GLOB_RECURSE HPP CONFIGURE_DEPENDS "*.hpp")

file (GLOB_RECURSE ALL_FILES LIST_DIRECTORIES TRUE CONFIGURE_DEPENDS "*")
set(ALL_DIRS "")
foreach(FILE ${ALL_FILES})
	if(IS_DIRECTORY ${FILE})
		list(APPEND ALL_DIRS ${FILE})
	endif()
endforeach()

include_directories(${ALL_DIRS})

add_executable (${PROJECT_NAME} ${CPP} ${HPP})
target_link_libraries(${PROJECT_NAME} ${LIB})