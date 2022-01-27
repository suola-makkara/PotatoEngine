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
