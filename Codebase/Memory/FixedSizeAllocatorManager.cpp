#ifndef ORBIS
#include "FixedSizeAllocatorManager.h"

const size_t FixedSizeAllocatorManager::ALLOCATOR_BLOCK_SIZES[ALLOCATOR_BLOCK_SIZES_SIZE] = {
	ALLOCATOR_BLOCK_SIZES_DEF
};

bool FixedSizeAllocatorManager::ALLOCATOR_LOOKUP_INITIALIZED = false;

void FixedSizeAllocatorManager::InitialiseAllocatorBlockSizeLookupArray()
{
	unsigned char blockSizeIndex = 0;
	for (size_t i = 0; i <= ALLOCATOR_BLOCK_SIZES_LARGEST; ++i) {
		if (i > ALLOCATOR_BLOCK_SIZES[blockSizeIndex])
			++blockSizeIndex;

		ALLOCATOR_BLOCK_SIZE_LOOKUP[i] = blockSizeIndex;
	}
}

unsigned char FixedSizeAllocatorManager::ALLOCATOR_BLOCK_SIZE_LOOKUP[ALLOCATOR_BLOCK_SIZES_LARGEST + 1] = {};


const size_t FixedSizeAllocatorManager::ALLOCATOR_NUM_BLOCK_SIZES = ALLOCATOR_BLOCK_SIZES_SIZE;
const size_t FixedSizeAllocatorManager::ALLOCATOR_MAX_BLOCK_SIZE = ALLOCATOR_BLOCK_SIZES_LARGEST;


FixedSizeAllocatorManager::FixedSizeAllocatorManager(
    unsigned pageSize,
    unsigned alignment)
{
	if (!ALLOCATOR_LOOKUP_INITIALIZED) {
		ALLOCATOR_LOOKUP_INITIALIZED = true;
		InitialiseAllocatorBlockSizeLookupArray();
	}

	for (unsigned i = 0; i < ALLOCATOR_NUM_BLOCK_SIZES; ++i) {
		m_allocators[i].Reset(ALLOCATOR_BLOCK_SIZES[i], pageSize, alignment);
	}
}
#endif
