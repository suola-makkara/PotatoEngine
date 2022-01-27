#pragma once

#include "storage.hpp"
#include "program_stack.hpp"
#include "noncopyable.hpp"

class ProgramAllocator : public Noncopyable
{
public:
	ProgramAllocator(size_t stackMem, size_t staticMem);

	Storage allocate(int value);

	Storage allocateStatic(int value);

	void push();

	void pop();

private:
	ProgramStack stack;
	ProgramStack staticMemory;
};
