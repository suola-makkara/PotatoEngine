#include "script_stack.hpp"

#include "gtest/gtest.h"

#include <string>

TEST(StackTest, IntAllocation)
{
	Stack stack(10000);
	auto a = stack.allocateInt(123);
	auto b = stack.allocateInt(3333);
	auto c = stack.allocateInt(434893);

	EXPECT_EQ(*a, 123);
	EXPECT_EQ(*b, 3333);
	EXPECT_EQ(*c, 434893);

	EXPECT_EQ(b - a, 1);
	EXPECT_EQ(c - a, 2);
}

TEST(StackTest, FloatAllocation)
{
	Stack stack(10000);
	auto a = stack.allocateFloat(123.32f);
	auto b = stack.allocateFloat(3333.44f);
	auto c = stack.allocateFloat(434893.54f);

	EXPECT_EQ(*a, 123.32f);
	EXPECT_EQ(*b, 3333.44f);
	EXPECT_EQ(*c, 434893.54f);

	EXPECT_EQ(b - a, 1);
	EXPECT_EQ(c - a, 2);
}

TEST(StackTest, StringAllocation)
{
	Stack stack(10000);
	auto a = stack.allocateString("test");
	auto b = stack.allocateString("wwqweqe");
	auto c = stack.allocateString("bbbbb");

	EXPECT_EQ(std::string(a),"test");
	EXPECT_EQ(std::string(b), "wwqweqe");
	EXPECT_EQ(std::string(c), "bbbbb");

	EXPECT_EQ(b - a, 5);
	EXPECT_EQ(c - b, 8);
}

TEST(StackTest, MultipleTypesAllocation)
{
	Stack stack(10000);
	auto a = stack.allocateString("test");
	auto b = stack.allocateInt(10);
	auto c = stack.allocateFloat(1234.0f);

	EXPECT_EQ(std::string(a), "test");
	EXPECT_EQ(*b, 10);
	EXPECT_EQ(*c, 1234.0f);
}

TEST(StackTest, PushPopTest)
{
	Stack stack(10000);
	auto a = stack.allocateString("test");
	stack.push();
	auto b = stack.allocateFloat(123.0333f);
	EXPECT_EQ(*b, 123.0333f);
	stack.pop();
	auto c = stack.allocateString("eeee");

	EXPECT_EQ(std::string(a), "test");
	EXPECT_EQ(std::string(c), "eeee");

	EXPECT_EQ(c - a, 5);
}
