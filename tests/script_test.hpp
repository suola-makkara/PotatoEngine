#include "script_runner.hpp"

#include "gtest/gtest.h"

#include <ostream>

TEST(ScriptTest, StringTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print("test"))");

	EXPECT_EQ(outStream.str(), "test");
}

TEST(ScriptTest, IntegerAdditionTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(123 + 99934))");

	EXPECT_EQ(outStream.str(), "100057");
}

TEST(ScriptTest, IntegerSubtractionTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(2783 - 12903))");

	EXPECT_EQ(outStream.str(), "-10120");
}

TEST(ScriptTest, IntegerMultiplicationTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(2783 * 12903))");

	EXPECT_EQ(outStream.str(), "35909049");
}

TEST(ScriptTest, IntegerDivisionTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(12903 / 2384))");

	EXPECT_EQ(outStream.str(), "5");
}

TEST(ScriptTest, OrderOfOperationsTest1)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(1 + 2 * 5))");

	EXPECT_EQ(outStream.str(), "11");
}

TEST(ScriptTest, OrderOfOperationsTest2)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print(3 * 5 / 2))");

	EXPECT_EQ(outStream.str(), "7");
}

TEST(ScriptTest, ParenthesesTest)
{
	std::ostringstream outStream;
	ScriptRunner scriptRunner(&outStream);
	scriptRunner.run(R"(print((1 + 2) * 5))");

	EXPECT_EQ(outStream.str(), "15");
}
