#include "storage.hpp"

#include "gtest/gtest.h"

#include <string>
#include <memory>

TEST(StorageTest, FullTest)
{
	Storage storage(10'000, 1'000);
	auto s0 = storage.allocateStorage<int>(0, 10);
	auto s1 = storage.allocateStorage<int>(1, 5);
	auto s2 = storage.allocateStorage<int>(1, 2);
	auto s3 = storage.allocateStorage<int>(2, 3);

	ASSERT_EQ(s0->type, StorageType::INTEGER);
	ASSERT_EQ(s2->type, StorageType::INTEGER);
	ASSERT_EQ(s3->type, StorageType::INTEGER);

	ASSERT_EQ(s0->size, 10);
	ASSERT_EQ(s2->size, 2);
	ASSERT_EQ(s3->size, 3);

	for (int i = 0; i < 10; i++)
		reinterpret_cast<int*>(s0->valuePtr)[i] = 10 * i;
	for (int i = 0; i < 2; i++)
		reinterpret_cast<int*>(s2->valuePtr)[i] = 1234 * i;
	for (int i = 0; i < 3; i++)
		reinterpret_cast<int*>(s3->valuePtr)[i] = 359 * i;

	for (int i = 0; i < 10; i++)
		EXPECT_EQ(reinterpret_cast<int*>(s0->valuePtr)[i], 10 * i);
	for (int i = 0; i < 2; i++)
		EXPECT_EQ(reinterpret_cast<int*>(s2->valuePtr)[i], 1234 * i);
	for (int i = 0; i < 3; i++)
		EXPECT_EQ(reinterpret_cast<int*>(s3->valuePtr)[i], 359 * i);

	EXPECT_EQ(s0, storage.getStorage(0));
	EXPECT_EQ(s2, storage.getStorage(1));
	EXPECT_EQ(s3, storage.getStorage(2));
}
