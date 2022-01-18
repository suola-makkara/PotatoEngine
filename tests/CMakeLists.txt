cmake_minimum_required (VERSION 3.15)

include(GoogleTest)

add_executable(test_test test_test.cpp)
target_link_libraries(test_test gtest)
gtest_add_tests(TARGET test_test TEST_SUFFIX "" TEST_LIST "")
