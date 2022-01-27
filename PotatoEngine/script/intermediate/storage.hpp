#pragma once

#include <cstddef>
#include <memory>
#include <vector>

enum class StorageType
{
	INTEGER,
	FLOAT
};

using ValuePtr = void*;

struct StorageUnit
{
	size_t size;
	StorageType type;
	ValuePtr valuePtr;
};

using StorageIndex = size_t;
using StorageLocation = std::byte*;

class Storage
{
public:
	Storage(size_t mem, size_t maxAllocations)
		: memory(std::make_unique<std::byte[]>(mem)), storageLocations(maxAllocations)
	{
		storageLocations[0] = memory.get();
	}

	template<typename T>
	StorageUnit* allocateStorage(StorageIndex index, size_t size)
	{
		static_assert(0);
		return nullptr;
	}

	template<>
	StorageUnit* allocateStorage<int>(StorageIndex index, size_t size)
	{
		 auto header = allocateInternal<int>(index, size);
		 header->type = StorageType::INTEGER;
		 return header;
	}

	template<>
	StorageUnit* allocateStorage<float>(StorageIndex index, size_t size)
	{
		auto header = allocateInternal<float>(index, size);
		header->type = StorageType::FLOAT;
		return header;
	}

	template<>
	StorageUnit* allocateStorage<void>(StorageIndex index, size_t size)
	{
		storageLocations[index + 1] = storageLocations[index];
		return nullptr;
	}

	StorageUnit* getStorage(StorageIndex index)
	{
		return reinterpret_cast<StorageUnit*>(storageLocations[index]);
	}

	std::unique_ptr<std::byte[]> memory;
	std::vector<StorageLocation> storageLocations;

private:
	template<typename T>
	inline StorageUnit* allocateInternal(size_t index, size_t size)
	{
		StorageUnit* header = new (storageLocations[index]) StorageUnit;
		header->valuePtr = new (storageLocations[index] + sizeof(StorageUnit)) T[size];
		storageLocations[index + 1] = storageLocations[index] + sizeof(StorageUnit) + size * sizeof(T);
		header->size = size;
		return header;
	}
};
