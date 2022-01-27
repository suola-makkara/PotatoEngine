#pragma once

#include "storage.hpp"

class VirtualProgram;

void test10(VirtualProgram* programPtr, std::vector<StorageIndex>& args);

void alloc(VirtualProgram* programPtr, std::vector<StorageIndex>& args);

class Instruction
{
public:
	Instruction() = default;

	virtual void execute() = 0;

	VirtualProgram* programPtr = nullptr;
};

class CFunctionCall : public Instruction
{
public:
	using Func = void (*)(VirtualProgram*, std::vector<StorageIndex>&);
	std::vector<StorageIndex> args;
	Func func;

	CFunctionCall(Func func, std::vector<StorageIndex>&& args);

	void execute() override;
};

class PushStack : public Instruction
{
public:
	PushStack() = default;

	void execute() override;
};

class PopStack : public Instruction
{
public:
	PopStack() = default;

	void execute() override;
};
