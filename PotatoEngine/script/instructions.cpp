#pragma once

#include "virtual_program.hpp"
#include "program_allocator.hpp"
#include "storage.hpp"
#include "instructions.hpp"

#include <vector>
#include <tuple>
#include <array>

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
}


CFunctionCall::CFunctionCall(Func func, Storage* out, std::vector<Storage*>&& args)
		: func(func), out(out), args(std::move(args)) { }

void CFunctionCall::execute()
{
	*out = func(&programPtr->allocator, args);
}
