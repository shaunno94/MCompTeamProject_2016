#pragma once

#include "FixedSizeAllocator.h"
#include <cassert>

// Extracting this information from a static array is unreliable, so separate defines were made
#define ALLOCATOR_BLOCK_SIZES_SIZE 47
#define ALLOCATOR_BLOCK_SIZES_DEF \
/* 4-increments*/ \
4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, \
52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,\
/* 32-increments*/ \
128, 160, 192, 224, 256, 288, 320, 352, 384, \
416, 448, 480, 512, 544, 576, 608, 640, \
/* 64-increments*/\
704, 768, 832, 896, 960, 1024
#define ALLOCATOR_BLOCK_SIZES_LARGEST 1024


class FixedSizeAllocatorManager
{
private:
	static const size_t ALLOCATOR_BLOCK_SIZES[ALLOCATOR_BLOCK_SIZES_SIZE];
	static unsigned char ALLOCATOR_BLOCK_SIZE_LOOKUP[ALLOCATOR_BLOCK_SIZES_LARGEST + 1];
	static void InitialiseAllocatorBlockSizeLookupArray();
	static bool ALLOCATOR_LOOKUP_INITIALIZED;

public:
	static const size_t ALLOCATOR_NUM_BLOCK_SIZES;
	static const size_t ALLOCATOR_MAX_BLOCK_SIZE;

	FixedSizeAllocator m_allocators[ALLOCATOR_BLOCK_SIZES_SIZE];

	FixedSizeAllocatorManager(
	    unsigned pageSize = FixedSizeAllocator::DEFAULT_PAGE_SIZE,
	    unsigned alignment = FixedSizeAllocator::DEFAULT_ALIGNMENT);

	inline FixedSizeAllocator* GetAllocator(size_t size)
	{
		assert(("FixedSizeAllocatorManager::GetAllocator provided size was too large", size <= ALLOCATOR_MAX_BLOCK_SIZE));
		return &m_allocators[ALLOCATOR_BLOCK_SIZE_LOOKUP[size]];
	}

	inline void* Allocate(size_t size)
	{
		return GetAllocator(size)->Allocate();
	}

	inline void Free(void* p, size_t size)
	{
		GetAllocator(size)->Free(p);
	}


	/// <summary>
	///
	/// </summary>
	/// <example>
	/// <code>
	/// FixedSizeAllocatorManager manager;
	/// int* a = manager.New<int>();
	/// </code>
	/// </example>
	template <typename T>
	inline T* New()
	{
		return new (Allocate(sizeof(T))) T();
	}

	/// <summary>
	///
	/// </summary>
	/// <example>
	/// <code>
	/// FixedSizeAllocatorManager manager;
	/// std::vector<int>* a = manager.New<std::vector<int>>(100);
	/// </code>
	/// </example>
	template <typename T, typename A0>
	inline T* New(A0 a0)
	{
		return new (Allocate(sizeof(T))) T(a0);
	}

	template <typename T, typename A0, typename A1>
	inline T* New(A0 a0, A1 a1)
	{
		return new (Allocate(sizeof(T))) T(a0, a1);
	}

	template <typename T, typename A0, typename A1, typename A2>
	inline T* New(A0 a0, A1 a1, A2 a2)
	{
		return new (Allocate(sizeof(T))) T(a0, a1, a2);
	}

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	inline T* New(A0 a0, A1 a1, A2 a2, A3 a3)
	{
		return new (Allocate(sizeof(T))) T(a0, a1, a2, a3);
	}

	template <typename T>
	inline void Delete(T* p)
	{
		p->~T();
		Free(p, sizeof(T));
	}

};
