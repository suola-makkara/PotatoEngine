#include "virtual_program.hpp"

#include "gtest/gtest.h"

#include <string>
#include <memory>

TEST(VirtualProgramTest, StorageAndAllocationTest)
{
	VirtualProgram vp;

	auto s0 = vp.addInstruction(std::make_unique<CFunctionCall>(test10, std::vector<StorageIndex>()));
	auto s1 = vp.addInstruction(std::make_unique<CFunctionCall>(alloc, std::vector<StorageIndex>{s0}));

	vp.run();

	auto p0 = vp.getStackStorage(s0);
	auto p1 = vp.getStackStorage(s1);

	ASSERT_EQ(p0->size, 1);
	ASSERT_EQ(p1->size, 1);

	ASSERT_EQ(p0->type, StorageType::INTEGER);
	ASSERT_EQ(p1->type, StorageType::INTEGER);

	EXPECT_EQ(*reinterpret_cast<int*>(p0->valuePtr), 10);
	EXPECT_EQ(*reinterpret_cast<int*>(p1->valuePtr), 10);
}
