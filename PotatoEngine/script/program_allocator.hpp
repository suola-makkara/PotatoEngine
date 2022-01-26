#pragma once

#include "storage.hpp"
#include "program_stack.hpp"
#include "noncopyable.hpp"

class ProgramAllocator : public Noncopyable
{
public:
	ProgramAllocator(size_t stackMem)//, size_t readonlyMem)
		: stack(stackMem) { } //, readonly(readonlyMem) { }

	Storage allocate(int value)
	{
		auto ptr = stack.allocate<int>();
		*ptr = value;

		return Storage(Storage::StorageType::INTEGER, ptr, 1);
	}

	//void heapAllocate(Value* target, int value);

private:
	ProgramStack stack;
	//ProgramStack readonly;
};