#include "virtual_program.hpp"

VirtualProgram::VirtualProgram()
	: stackStorage(10'000, 1'000), stackStorageIndex{}, instructionPtr{} { }

void VirtualProgram::run()
{
	instructionPtr = 0;
	stackStorageIndex = 0;

	while (instructionPtr < instructions.size())
	{
		instructions[instructionPtr]->execute();
		instructionPtr++;
	}
}

StorageUnit* VirtualProgram::getStackStorage(StorageIndex index)
{
	return stackStorage.getStorage(index);
}

StorageIndex VirtualProgram::addInstruction(std::unique_ptr<Instruction>&& instruction)
{
	instruction->programPtr = this;
	instructions.push_back(std::move(instruction));
	return -1;
}

StorageIndex VirtualProgram::addInstruction(std::unique_ptr<CFunctionCall>&& instruction)
{
	addInstruction(std::unique_ptr<Instruction>(std::move(instruction)));
	return stackStorageIndex++;
}

StorageIndex VirtualProgram::addInstruction(std::unique_ptr<PushStack>&& instruction)
{
	addInstruction(std::unique_ptr<Instruction>(std::move(instruction)));
	return -1;
}

StorageIndex VirtualProgram::addInstruction(std::unique_ptr<PopStack>&& instruction)
{
	addInstruction(std::unique_ptr<Instruction>(std::move(instruction)));
	return -1;
}
