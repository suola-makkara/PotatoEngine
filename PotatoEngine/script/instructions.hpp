#pragma once

#include "virtual_program.hpp"
#include "program_allocator.hpp"
#include "storage.hpp"

#include <vector>
#include <tuple>
#include <array>

Storage assign(ProgramAllocator* alloc, std::vector<Storage*>& args);

Storage add(ProgramAllocator* alloc, std::vector<Storage*>& args);

class Instruction
{
public:
	Instruction() = default;

	virtual void execute() = 0;

	VirtualProgram* programPtr;
};

class CFunctionCall : public Instruction
{
public:
	using Func = Storage (*)(ProgramAllocator*, std::vector<Storage*>&);
	std::vector<Storage*> args;
	Storage* out;
	Func func;

	CFunctionCall(Func func, Storage* out, std::vector<Storage*>&& args);

	void execute() override;
};
