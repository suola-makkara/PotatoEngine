#include "tokenizer.hpp"

#include "gtest/gtest.h"

TEST(TokenizeTest, EmptyTest)
{
	EXPECT_EQ(Tokenizer::tokenize("", std::set<char>(), std::set<char>(), std::set<std::string>(), 0).size(), 0);
}

TEST(TokenizeTest, SimpleTest)
{
	auto ret = Tokenizer::tokenize("abaaaba", std::set<char>{'a', 'b'}, std::set<char>{'a'}, std::set<std::string>(), 0);

	ASSERT_EQ(ret.size(), 2);

	EXPECT_EQ(ret[0], "b");
	EXPECT_EQ(ret[1], "b");
}

TEST(TokenizeTest, SimpleTestWithTokens)
{
	auto ret = Tokenizer::tokenize("abaab", std::set<char>{'a', 'b'}, std::set<char>{}, std::set<std::string>{"aa"}, 2);

	ASSERT_EQ(ret.size(), 3);

	EXPECT_EQ(ret[0], "ab");
	EXPECT_EQ(ret[1], "aa");
	EXPECT_EQ(ret[2], "b");
}

TEST(TokenizeTest, FullTest)
{
	auto ret = Tokenizer::tokenize("5 == 2- 222>=25123", std::set<char>{' ', '1', '3', '5', '2', '-', '=', '>', '0'},
		std::set<char>{' '}, std::set<std::string>{"==", "=", ">=", "-", ">", "123"}, 3);

	ASSERT_EQ(ret.size(), 8);

	EXPECT_EQ(ret[0], "5");
	EXPECT_EQ(ret[1], "==");
	EXPECT_EQ(ret[2], "2");
	EXPECT_EQ(ret[3], "-");
	EXPECT_EQ(ret[4], "222");
	EXPECT_EQ(ret[5], ">=");
	EXPECT_EQ(ret[6], "25");
	EXPECT_EQ(ret[7], "123");
}
