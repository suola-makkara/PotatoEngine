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

//#pragma once
//
//#include "program_allocator.hpp"
//#include "storage.hpp"
//
//#include <vector>
//#include <tuple>
//#include <array>
//
//Storage assign(ProgramAllocator* alloc, std::vector<Storage*>& args);
//
//Storage add(ProgramAllocator* alloc, std::vector<Storage*>& args);
//
//Storage println(ProgramAllocator* alloc, std::vector<Storage*>& args);
//
//using InstrPtr = size_t;
//
//class Instruction
//{
//public:
//	Instruction() = default;
//
//	virtual void execute() = 0;
//
//	class VirtualProgram* programPtr = nullptr;
//};
//
//class CFunctionCall : public Instruction
//{
//public:
//	using Func = Storage (*)(ProgramAllocator*, std::vector<Storage*>&);
//	std::vector<Storage*> args;
//	Storage* out;
//	Func func;
//
//	CFunctionCall(Func func, Storage* out, std::vector<Storage*>&& args);
//
//	void execute() override;
//};
//
//class Jump : public Instruction
//{
//public:
//	InstrPtr jumpPtr;
//
//	Jump(InstrPtr jumpPtr);
//
//	void execute() override;
//};
//
//class FunctionCall : public Instruction
//{
//public:
//	InstrPtr entrancePtr;
//
//	FunctionCall(InstrPtr entrancePtr);
//
//	void execute() override;
//};
//
//class FunctionReturn : public Instruction
//{
//public:
//	FunctionReturn() = default;
//
//	void execute() override;
//};
//
//class NoOp : public Instruction
//{
//public:
//	NoOp() = default;
//
//	void execute() override { }
//};