#include "virtual_program.hpp"
#include "instructions.hpp"

#include "gtest/gtest.h"

#include <string>
#include <memory>

TEST(VirtualProgramTest, StorageAndAllocationTest)
{
	VirtualProgram vp;

	auto s0 = vp.createStorage();
	auto s1 = vp.createStorage();
	auto s2 = vp.createStorage();

	*s0 = vp.allocator.allocate(10);
	*s1 = vp.allocator.allocate(20);

	vp.appendInstruction(std::make_unique<CFunctionCall>(add, s2, std::vector{ s0, s1 }));
	vp.run();

	ASSERT_EQ(s0->type, Storage::StorageType::INTEGER);
	ASSERT_EQ(s1->type, Storage::StorageType::INTEGER);
	ASSERT_EQ(s2->type, Storage::StorageType::INTEGER);

	EXPECT_EQ(*reinterpret_cast<int*>(s0->valuePtr), 10);
	EXPECT_EQ(*reinterpret_cast<int*>(s1->valuePtr), 20);
	EXPECT_EQ(*reinterpret_cast<int*>(s2->valuePtr), 30);
}
