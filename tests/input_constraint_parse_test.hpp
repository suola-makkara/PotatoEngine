#include "input_constraint_parser.hpp"

#include "gtest/gtest.h"
#include "GLFW/glfw3.h"

#include <string>

TEST(InputConstraintParseTest, SingleKey)
{
	const std::string SINGLE_KEY_INPUT("(A)[]{}");

	auto constraint = InputConstraintParser::parseInputConstraint(SINGLE_KEY_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 1);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_A));
	EXPECT_EQ(constraint.command, "");
}

TEST(InputConstraintParseTest, MultiKey)
{
	const std::string MULTI_KEY_INPUT("(A B C)[]{}");

	auto constraint = InputConstraintParser::parseInputConstraint(MULTI_KEY_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 3);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_A));
	EXPECT_EQ(constraint.inputSeq[1], Event(Event::Type::KEY_RELEASE, GLFW_KEY_B));
	EXPECT_EQ(constraint.inputSeq[2], Event(Event::Type::KEY_RELEASE, GLFW_KEY_C));
	EXPECT_EQ(constraint.command, "");
}

TEST(InputConstraintParseTest, ModifiedKey)
{
	const std::string MODIFIED_KEY_INPUT("(CTRL+A)[]{}");

	auto constraint = InputConstraintParser::parseInputConstraint(MODIFIED_KEY_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 1);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_A, GLFW_MOD_CONTROL));
	EXPECT_EQ(constraint.command, "");
}

TEST(InputConstraintParseTest, MultiModifiedKey)
{
	const std::string MULTI_MODIFIED_KEY_INPUT("(SHIFT+X CTRL+A)[]{}");

	auto constraint = InputConstraintParser::parseInputConstraint(MULTI_MODIFIED_KEY_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 2);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_X, GLFW_MOD_SHIFT));
	EXPECT_EQ(constraint.inputSeq[1], Event(Event::Type::KEY_RELEASE, GLFW_KEY_A, GLFW_MOD_CONTROL));
	EXPECT_EQ(constraint.command, "");
}

TEST(InputConstraintParseTest, CommandTest)
{
	const std::string COMMAND_INPUT("(W)[]{test command}");

	auto constraint = InputConstraintParser::parseInputConstraint(COMMAND_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 1);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_W));
	EXPECT_EQ(constraint.command, "test command");
}

TEST(InputConstraintParseTest, RequirmentTest)
{
	const std::string REQUIRMENT_INPUT("(O)[test requirment]{test command}");

	auto constraint = InputConstraintParser::parseInputConstraint(REQUIRMENT_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 1);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_O));
	EXPECT_EQ(constraint.command, "if(test_requirment){}");
}

TEST(InputConstraintParseTest, FullTest)
{
	const std::string REQUIRMENT_INPUT("(CTRL+J)[test requirment]{test command}");

	auto constraint = InputConstraintParser::parseInputConstraint(REQUIRMENT_INPUT);

	ASSERT_EQ(constraint.inputSeq.size(), 1);
	EXPECT_EQ(constraint.inputSeq[0], Event(Event::Type::KEY_RELEASE, GLFW_KEY_J, GLFW_MOD_CONTROL));
	EXPECT_EQ(constraint.command, "if(test_requirment){test command}");
}
