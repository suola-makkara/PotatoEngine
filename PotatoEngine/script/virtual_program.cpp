#include "virtual_program.hpp"
#include "instructions.hpp"

void VirtualProgram::run()
{
	while (instructionPtr < instructions.size())
	{
		instructions[instructionPtr]->execute();
		instructionPtr++;
	}
}

Storage* VirtualProgram::createStorage()
{
	storage.push_back(Storage(Storage::StorageType::NONE));
	return &storage.back();
}

void VirtualProgram::appendInstruction(std::unique_ptr<Instruction>&& instruction)
{
	instruction->programPtr = this;
	instructions.push_back(std::move(instruction));
}
