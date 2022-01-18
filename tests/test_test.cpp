#include "gtest/gtest.h"

TEST(TestTest, TestXXX)
{
	EXPECT_EQ(1, 1);
}

TEST(TestTest, TestYYY)
{
	EXPECT_EQ(1, 0);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}