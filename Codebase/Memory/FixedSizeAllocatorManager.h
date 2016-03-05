#ifndef ORBIS
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


#ifdef _DEBUG
#define ALOCATOR_TRACK_STATS
#endif


/// @ingroup Memory
/// <summary>
/// A suballocator manager class that uses several <see cref="FixedSizeAllocator"/> objects to allocate different size memory blocks.
/// </summary>
class FixedSizeAllocatorManager
{
private:
	/// <summary>
	/// Array of <see cref="FixedSizeAllocator"/> block sizes.
	/// </summary>
	static const unsigned int ALLOCATOR_BLOCK_SIZES[ALLOCATOR_BLOCK_SIZES_SIZE];
	/// <summary>
	/// Mapping memory block sizes to <see cref="FixedSizeAllocator"/> with the closest block size.
	/// </summary>
	static unsigned char ALLOCATOR_BLOCK_SIZE_LOOKUP[ALLOCATOR_BLOCK_SIZES_LARGEST + 1];
	/// <summary>
	/// <see cref="FixedSizeAllocatorManager::ALLOCATOR_BLOCK_SIZE_LOOKUP"/> initializer function.
	/// </summary>
	static void InitialiseAllocatorBlockSizeLookupArray();
	/// <summary>
	/// <see cref="FixedSizeAllocatorManager::ALLOCATOR_BLOCK_SIZE_LOOKUP"/> initialization tracker.
	/// </summary>
	static bool ALLOCATOR_LOOKUP_INITIALIZED;

public:
	/// <summary>
	/// Number of available <see cref="FixedSizeAllocator"/> instances of different block sizes.
	/// </summary>
	static const size_t ALLOCATOR_NUM_BLOCK_SIZES;
	/// <summary>
	/// The size of the largest <see cref="FixedSizeAllocator"/> block size.
	/// </summary>
	static const size_t ALLOCATOR_MAX_BLOCK_SIZE;
	/// <summary>
	/// Array of <see cref="FixedSizeAllocator"/> of different block sizes.
	/// </summary>
	FixedSizeAllocator m_allocators[ALLOCATOR_BLOCK_SIZES_SIZE];

	/// <summary>
	/// The class constructor.
	/// </summary>
	/// <param name="pageSize">Size of a managed continious block of memory.</param>
	/// <param name="alignment">Memory alignment for memory blocks.</param>
	FixedSizeAllocatorManager(
	  unsigned pageSize = FixedSizeAllocator::DEFAULT_PAGE_SIZE,
	  unsigned alignment = FixedSizeAllocator::DEFAULT_ALIGNMENT);

	/// <summary>
	/// Helper method to get a <see cref="FixedSizeAllocator"/> mapped to a given memory block size.
	/// </summary>
	/// <param name="size">Size of the memory block.</param>
	/// <returns>Pointer to a <see cref="FixedSizeAllocator"/> for the given block size.</returns>
	inline FixedSizeAllocator* GetAllocator(size_t size)
	{
		assert(("FixedSizeAllocatorManager::GetAllocator provided size was too large", size <= ALLOCATOR_MAX_BLOCK_SIZE));
		return &m_allocators[ALLOCATOR_BLOCK_SIZE_LOOKUP[size]];
	}

	/// <summary>
	/// Method for manually specifying information for managed memory allocation.
	/// </summary>
	/// <param name="size">Size of a memory block to allocate.</param>
	/// <returns>Pointer to the new block of memory.</returns>
	inline void* Allocate(size_t size)
	{
		return GetAllocator(size)->Allocate();
	}

	/// <summary>
	/// Method for manually specifying information for managed memory deletion.
	/// </summary>
	/// <param name="p">Pointer to an object instance.</param>
	/// <param name="size">Size of the managed memory block to delete.</param>
	inline void Free(void* p, size_t size)
	{
		GetAllocator(size)->Free(p);
	}


	template <typename T>
	/// <summary>
	///	Method for instantiation using the managed memory.
	/// </summary>
	/// <example>
	/// <code>
	/// FixedSizeAllocatorManager manager;
	/// int* a = manager.New<int>();
	/// </code>
	/// </example>
	/// <returns>Pointer to the new instance.</returns>
	inline T* New()
	{
		return new (Allocate(sizeof(T))) T();
	}

	template <typename T, typename A0>
	/// <summary>
	///	Method for instantiation using the managed memory.
	/// </summary>
	/// <example>
	/// <code>
	/// FixedSizeAllocatorManager manager;
	/// std::vector<int>* a = manager.New<std::vector<int>>(100);
	/// </code>
	/// </example>
	/// <param name="a0">Constructor argument.</param>
	/// <returns>Pointer to the new instance.</returns>
	inline T* New(A0 a0)
	{
		return new (Allocate(sizeof(T))) T(a0);
	}

	template <typename T, typename A0, typename A1>
	/// <summary>
	/// Method for instantiation using the managed memory.
	/// </summary>
	/// <param name="a0">Constructor argument.</param>
	/// <param name="a1">Constructor argument.</param>
	/// <returns>Pointer to the new instance.</returns>
	inline T* New(A0 a0, A1 a1)
	{
		return new (Allocate(sizeof(T))) T(a0, a1);
	}

	template <typename T, typename A0, typename A1, typename A2>
	/// <summary>
	/// Method for instantiation using the managed memory.
	/// </summary>
	/// <param name="a0">Constructor argument.</param>
	/// <param name="a1">Constructor argument.</param>
	/// <param name="a2">Constructor argument.</param>
	/// <returns>Pointer to the new instance.</returns>
	inline T* New(A0 a0, A1 a1, A2 a2)
	{
		return new (Allocate(sizeof(T))) T(a0, a1, a2);
	}

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	/// <summary>
	/// Method for instantiation using the managed memory.
	/// </summary>
	/// <param name="a0">Constructor argument.</param>
	/// <param name="a1">Constructor argument.</param>
	/// <param name="a2">Constructor argument.</param>
	/// <param name="a3">Constructor argument.</param>
	/// <returns>Pointer to the new instance.</returns>
	inline T* New(A0 a0, A1 a1, A2 a2, A3 a3)
	{
		return new (Allocate(sizeof(T))) T(a0, a1, a2, a3);
	}

	template <typename T>
	/// <summary>
	/// Destructor for managed memory.
	/// </summary>
	/// <remarks>
	/// This is not a safe destructor.
	/// Down-casted pointers will be interpreted as instances of that type and might not be correctly matched to a <see cref="FixedSizeAllocator"/>.
	/// </remarks>
	/// <param name="p">Pointer to an object instance.</param>
	inline void Delete(T* p)
	{
		p->~T();
		Free(p, sizeof(T));
	}


#ifdef ALOCATOR_TRACK_STATS
	inline uint64_t GetAllocatedMemory()
	{
		uint64_t result = 0;
		for (unsigned int i = 0; i < ALLOCATOR_BLOCK_SIZES_SIZE; ++i)
			result += m_allocators[i].GetAllocatedMemory();
		return result;
	}

	inline uint64_t GetUsedMemory()
	{
		uint64_t result = 0;
		for (unsigned int i = 0; i < ALLOCATOR_BLOCK_SIZES_SIZE; ++i)
			result += m_allocators[i].GetUsedMemory();
		return result;
	}
#endif

};
#endif