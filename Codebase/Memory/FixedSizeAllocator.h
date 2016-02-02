#pragma once

#ifdef _DEBUG
#define ALOCATOR_FILL_MEMORY 0
#else
#define ALOCATOR_FILL_MEMORY 0
#endif

#define ALOCATOR_TRACK_STATS 0

#include <cstdint>

/// <summary>
/// A suballocator class for managing fixed size memory blocks
/// </summary>
/// <remarks>
/// The implementation is based on Ming-Lun "Allen" Chou's memory management post (http://allenchou.net/memory-management-series/)
/// </remarks>
class FixedSizeAllocator
{
public:
	static const size_t DEFAULT_PAGE_SIZE = 4096;
	static const size_t DEFAULT_ALIGNMENT = 4;

    /// <summary>
    /// Memory block tracking class.
    /// </summary>
    /// <remarks>
    /// This structure is union-ed with data
    /// </remarks>
    struct BlockHeader {
        /// <summary>
        /// Pointer to next BlockHeader.
        /// </summary>
        BlockHeader* Next;
    };

    /// <summary>
    /// Memory block tracking class.
    /// </summary>
    struct PageHeader {
        /// <summary>
        /// Pointer to the previous PageHeader.
        /// </summary>
        PageHeader*  Next;
    };

    /// <summary>
    /// Optional pattern to fill memory used for alignment.
    /// </summary>
    /// <remarks>
    /// Value 0xFC = 252 = 1111 1100
    /// </remarks>
    static const unsigned char PATTERN_ALIGN = 0xFC;

    /// <summary>
    /// Optional pattern to fill memory that has been allocated.
    /// </summary>
    /// <remarks>
    /// Value 0xFD = 253 = 1111 1101
    /// </remarks>
    static const unsigned char PATTERN_ALLOC = 0xFD;

    /// <summary>
    /// Optional pattern to fill memory that has been freed.
    /// </summary>
    /// <remarks>
    /// Value 0xFE = 254 = 1111 1110
    /// </remarks>
    static const unsigned char PATTERN_FREE = 0xFE;

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="dataSize">Size of the fixed size data blocks.</param>
    /// <param name="pageSize">Size of the memory pages in bytes.</param>
    /// <param name="alignment">Alignment for data blocks.</param>
    FixedSizeAllocator (
		unsigned dataSize = 1024,
        unsigned pageSize = DEFAULT_PAGE_SIZE,
        unsigned alignment = DEFAULT_ALIGNMENT
    );

    /// <summary>
    /// Destructor.
    /// </summary>
    ~FixedSizeAllocator();

    /// <summary>
    /// Resets the allocator to a new configuration.
    /// </summary>
    /// <param name="dataSize">Size of the fixed size data blocks.</param>
    /// <param name="pageSize">Size of the memory pages in bytes.</param>
    /// <param name="alignment">Alignment for data blocks.</param>
    void Reset (
        unsigned dataSize,
        unsigned pageSize = 4096,
        unsigned alignment = 4
    );

    /// <summary>
    /// Allocates a block of memory.
    /// </summary>
    /// <returns>Pointer to an allocated memory block.</returns>
    void* Allocate();

    /// <summary>
    /// Deallocates a block of memory.
    /// </summary>
    /// <param name="p">Pointer to an allocated memory block.</param>
    void Free(void* p);

    /// <summary>
    /// Deallocates all memory.
    /// </summary>
    void FreeAll();

private:
    /// <summary>
    /// Helper function for getting alignment padding.
    /// </summary>
    /// <param name="x">Number to align.</param>
    /// <returns>Number for the required padding.</returns>
    inline unsigned GetAlignmentPadding(uintptr_t x) const
    {
        return (m_alignment - (x % m_alignment)) % m_alignment;
    }

    /// <summary>
    /// Fill a free page with debug patterns.
    /// </summary>
    /// <param name="p">Pointer to a page of memory.</param>
    void FillFreePage(PageHeader*  p);

    /// <summary>
    /// Fill a free block with debug patterns.
    /// </summary>
    /// <param name="p">Pointer to a memory block.</param>
    void FillFreeBlock(BlockHeader* p);

    /// <summary>
    /// Fill an allocated block with debug patterns.
    /// </summary>
    /// <param name="p">Pointer to a memory block.</param>
    void FillAllocatedBlock(BlockHeader* p);

    /// <summary>
    /// Helper function that gives the first block.
    /// </summary>
    /// <param name="p">Pointer to a page of memory.</param>
    /// <returns>
    /// Pointer to the first data block.
    /// </returns>
    inline BlockHeader* Blocks(PageHeader* p)
    {
        uintptr_t pageHeaderEnd = uintptr_t(p + 1);
        //moving first block to align memory address
        return reinterpret_cast<BlockHeader*>(pageHeaderEnd + GetAlignmentPadding(pageHeaderEnd));
    }

    /// <summary>
    /// Helper function for getting the next block.
    /// </summary>
    /// <param name="p">Pointer to a memory block.</param>
    /// <returns>
    /// Pointer to the next block.
    /// </returns>
    inline BlockHeader* NextBlock(BlockHeader* p)
    {
        return reinterpret_cast<BlockHeader*> ((uintptr_t)p + m_blockSize);
    }

    /// <summary>
    /// Pointer to the latest page of memory.
    /// </summary>
    PageHeader* m_pageList;

    /// <summary>
    /// Pointer to the latest free memory block.
    /// </summary>
    BlockHeader* m_freeList;

    // Size-related data.
    /// <summary>
    /// Memory alignment of data.
    /// </summary>
    unsigned m_alignment;
    /// <summary>
    /// Number of data memory blocks on a page.
    /// </summary>
    unsigned m_blocksPerPage;
    /// <summary>
    /// Size of a memory block with padding in bytes.
    /// </summary>
    unsigned m_blockSize;
    /// <summary>
    /// Size of the memory block padding in bytes for memory alignment.
    /// </summary>
    unsigned m_blockAlignmentPadding;
    /// <summary>
    /// Expected data size of memory blocks in bytes.
    /// </summary>
    unsigned m_dataSize;
    /// <summary>
    /// Size of the memory pages in bytes.
    /// </summary>
    unsigned m_pageSize;

#if ALOCATOR_TRACK_STATS
    // statistics
    unsigned m_numPages;
    unsigned m_numBlocks;
    unsigned m_numFreeBlocks;
#endif

    /// <summary>
    /// Disabled copy constructor.
    /// </summary>
    /// <param name="clone">Allocator instance.</param>
    FixedSizeAllocator(const FixedSizeAllocator& clone) = delete;

    /// <summary>
    /// Disabled assignment operator.
    /// </summary>
    /// <param name="rhs">Allocator instance.</param>
    /// <returns>Allocator value.</returns>
    FixedSizeAllocator& operator=(const FixedSizeAllocator& rhs) = delete;
};

