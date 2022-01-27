#include "instructions.hpp"
#include "virtual_program.hpp"

void test10(VirtualProgram* programPtr, std::vector<StorageIndex>& args)
{
	auto ptr = programPtr->allocateStackStorage<int>(1);
	*reinterpret_cast<int*>(ptr->valuePtr) = 10;
}

void alloc(VirtualProgram* programPtr, std::vector<StorageIndex>& args)
{
	StorageUnit* ptr = programPtr->getStackStorage(args[0]);
	switch (ptr->type)
	{
	case StorageType::INTEGER:
	{
		auto s0 = programPtr->allocateStackStorage<int>(ptr->size);
		for (int i = 0; i < ptr->size; i++)
			reinterpret_cast<int*>(s0->valuePtr)[i] = reinterpret_cast<int*>(ptr->valuePtr)[i];
		return;
		break;
	}
	}

	programPtr->allocateStackStorage<void>(0);
}

CFunctionCall::CFunctionCall(Func func, std::vector<StorageIndex>&& args)
	: func(func), args(std::move(args)) { }

void CFunctionCall::execute()
{
	func(programPtr, args);
}

void PushStack::execute()
{

}

void PopStack::execute()
{

}

/*#pragma once

#include "virtual_program.hpp"
#include "program_allocator.hpp"
#include "storage.hpp"
#include "instructions.hpp"

#include <vector>
#include <tuple>
#include <array>
#include <iostream>

Storage assign(ProgramAllocator* alloc, std::vector<Storage*>& args)
{
	auto& left = args[0];
	auto& right = args[1];
	switch (right->type)
	{
	case Storage::StorageType::INTEGER:
		*left = alloc->allocate(*reinterpret_cast<int*>(right->valuePtr));
		return *left;
		break;
	}
}

Storage add(ProgramAllocator* alloc, std::vector<Storage*>& args)
{
	auto& left = args[0];
	auto& right = args[1];
	switch (left->type)
	{
	case Storage::StorageType::INTEGER:
		if (right->type == Storage::StorageType::INTEGER)
		{
			const int lv = *reinterpret_cast<int*>(left->valuePtr);
			const int rv = *reinterpret_cast<int*>(right->valuePtr);
			return alloc->allocate(lv + rv);
		}
		break;
	}

	return Storage(Storage::StorageType::VOID);
}

Storage println(ProgramAllocator* alloc, std::vector<Storage*>& args)
{
	if (args[0]->type == Storage::StorageType::INTEGER)
		std::cout << *reinterpret_cast<int*>(args[0]) << '\n';

	return Storage(Storage::StorageType::VOID);
}


CFunctionCall::CFunctionCall(Func func, Storage* out, std::vector<Storage*>&& args)
		: func(func), out(out), args(std::move(args)) { }

void CFunctionCall::execute()
{
	*out = func(&programPtr->allocator, args);
}


Jump::Jump(InstrPtr jumpPtr) : jumpPtr(jumpPtr) { }

void Jump::execute()
{
	programPtr->instructionPtr = jumpPtr;
}


FunctionCall::FunctionCall(InstrPtr entrancePtr)
	: entrancePtr(entrancePtr) { }


void FunctionCall::execute()
{
	programPtr->returnPtrStack.push(programPtr->instructionPtr);
	programPtr->instructionPtr = entrancePtr;
	programPtr->allocator.push();
}

void FunctionReturn::execute()
{
	programPtr->instructionPtr = programPtr->returnPtrStack.top();
	programPtr->returnPtrStack.pop();
	programPtr->allocator.pop();
}
*/