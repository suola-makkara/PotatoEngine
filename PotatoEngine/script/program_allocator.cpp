/*#include "program_allocator.hpp"

ProgramAllocator::ProgramAllocator(size_t stackMem, size_t staticMem)
	: stack(stackMem), staticMemory(staticMem) { }

Storage ProgramAllocator::allocate(int value)
{
	auto ptr = stack.allocate<int>();
	*ptr = value;

	return Storage(Storage::StorageType::INTEGER, ptr);
}

Storage ProgramAllocator::allocateStatic(int value)
{
	auto ptr = staticMemory.allocate<int>();
	*ptr = value;

	return Storage(Storage::StorageType::INTEGER, ptr, 1, Storage::READONLY);
}

void ProgramAllocator::push()
{
	stack.push();
}

void ProgramAllocator::pop()
{
	stack.pop();
}
*/