#include "input_constraint_parse_test.hpp"
#include "tokenize_test.hpp"
#include "script_test.hpp"
#include "stack_test.hpp"
#include "virtual_program_test.hpp"
#include "storage_test.hpp"

#include "gtest/gtest.h"


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}