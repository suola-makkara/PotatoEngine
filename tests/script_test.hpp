#include "script_runner.hpp"

#include "gtest/gtest.h"

#include <ostream>

std::string runScript(const std::string& script)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(script);
	return outStream.str();
}

TEST(ScriptTest, StringTest)
{
	EXPECT_EQ(runScript(R"(print("test"))"), "test");
}

TEST(ScriptTest, IntegerAdditionTest)
{
	EXPECT_EQ(runScript(R"(print(123 + 99934))"), "100057");
}

TEST(ScriptTest, IntegerSubtractionTest)
{
	EXPECT_EQ(runScript(R"(print(2783 - 12903))"), "-10120");
}

TEST(ScriptTest, IntegerMultiplicationTest)
{
	EXPECT_EQ(runScript(R"(print(2783 * 12903))"), "35909049");
}

TEST(ScriptTest, IntegerDivisionTest)
{
	EXPECT_EQ(runScript(R"(print(12903 / 2384))"), "5");
}

TEST(ScriptTest, OrderOfOperationsTest1)
{
	EXPECT_EQ(runScript(R"(print(1 + 2 * 5))"), "11");
}

TEST(ScriptTest, OrderOfOperationsTest2)
{
	EXPECT_EQ(runScript(R"(print(3 * 5 / 2))"), "7");
}

TEST(ScriptTest, ParenthesesTest)
{
	EXPECT_EQ(runScript(R"(print((1 + 2) * 5))"), "15");
}

TEST(ScriptTest, UnaryOperatorTest)
{
	EXPECT_EQ(runScript(R"(print(-5))"), "-5");
	EXPECT_EQ(runScript(R"(print(+5))"), "5");
}

TEST(ScriptTest, VarTest1)
{
	EXPECT_EQ(runScript(R"(x = 10 print(x))"), "10");
}

TEST(ScriptTest, VarTest2)
{
	EXPECT_EQ(runScript(R"(x = 10 y = 123 print(x + y))"), "133");
}

TEST(ScriptTest, VarTest3)
{
	EXPECT_EQ(runScript(R"(x = 10 y = 123 x = x + 5 print(x + y))"), "138");
}
