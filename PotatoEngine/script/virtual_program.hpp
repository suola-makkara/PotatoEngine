#pragma once

#include "program_allocator.hpp"
#include "storage.hpp"

#include <vector>
#include <memory>
#include <list>

class Instruction;

class VirtualProgram
{
public:
	VirtualProgram() : allocator(10'000), instructionPtr{} { }

	void run();

	Storage* createStorage();

	void appendInstruction(std::unique_ptr<Instruction>&& instruction);



	std::list<Storage> storage;

	std::vector<std::unique_ptr<Instruction>> instructions;
	size_t instructionPtr;

	ProgramAllocator allocator;
};
