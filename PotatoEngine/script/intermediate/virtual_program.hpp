#pragma once

#include "storage.hpp"
#include "instructions.hpp"

#include <stack>
#include <memory>


class VirtualProgram
{
public:
	VirtualProgram();

	template<typename T>
	StorageUnit* allocateStackStorage(size_t size)
	{
		return stackStorage.allocateStorage<T>(stackStorageIndex++, size);
	}

	//template<typename T>
	//StorageIndex allocateStaticStorage(size_t size)
	//{
	//	staticStorage.allocateStorage<T>(staticStorageIndex, size);
	//	return staticStorageIndex++;
	//}

	void run();

	StorageUnit* getStackStorage(StorageIndex index);

	StorageIndex addInstruction(std::unique_ptr<Instruction>&& instruction);

	StorageIndex addInstruction(std::unique_ptr<CFunctionCall>&& instruction);

	StorageIndex addInstruction(std::unique_ptr<PushStack>&& instruction);

	StorageIndex addInstruction(std::unique_ptr<PopStack>&& instruction);

private:
	Storage stackStorage;
	//Storage staticStorage;

	StorageIndex stackStorageIndex;
	//StorageIndex staticStorageIndex;

	std::stack<StorageIndex> storageIndices;

	std::vector<std::unique_ptr<Instruction>> instructions;

	size_t instructionPtr;
};
