cmake_minimum_required (VERSION 3.15)

include(GoogleTest)

set (LIB
	gtest
	PotatoEngineLib
)

add_executable(ParseTest "input_constraint_parse_test.cpp")
target_link_libraries(ParseTest ${LIB})
gtest_add_tests(TARGET ParseTest TEST_SUFFIX "" TEST_LIST "")
