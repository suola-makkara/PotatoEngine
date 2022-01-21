cmake_minimum_required (VERSION 3.15)

include(GoogleTest)

set (LIB
	gtest
	PotatoEngineLib
)

add_executable(PotatoEngineTest "test_main.cpp" "script_test.hpp")
target_link_libraries(PotatoEngineTest ${LIB})
gtest_add_tests(TARGET PotatoEngineTest TEST_SUFFIX "" TEST_LIST "")
