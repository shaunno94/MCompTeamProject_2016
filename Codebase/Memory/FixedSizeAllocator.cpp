#include "FixedSizeAllocator.h"

#include <algorithm>

FixedSizeAllocator::FixedSizeAllocator (
    unsigned dataSize,
    unsigned pageSize,
    unsigned alignment
) :
    m_pageList(nullptr),
    m_freeList(nullptr)
{
    Reset(dataSize, pageSize, alignment);
}

FixedSizeAllocator::~FixedSizeAllocator()
{
    FreeAll();
}

void FixedSizeAllocator::Reset (
    unsigned dataSize,
    unsigned pageSize,
    unsigned alignment)
{
    FreeAll();

    m_dataSize = dataSize;
    m_pageSize = pageSize;
    m_alignment = alignment;

    unsigned maxDataSize = std::max(sizeof(BlockHeader), m_dataSize);
    m_blockAlignmentPadding = GetAlignmentPadding(maxDataSize);
    m_blockSize = maxDataSize + m_blockAlignmentPadding;
    //subtracting PageHeaderSize and max page alignment padding (for aligning the first block address from given memory)
    m_blocksPerPage = (m_pageSize - sizeof(PageHeader) - (m_alignment - 1)) / m_blockSize;
}

void* FixedSizeAllocator::Allocate()
{
    // free list empty, create new page
    if (!m_freeList) {
        // allocate new page
        PageHeader* newPage = reinterpret_cast<PageHeader*>(new char[m_pageSize]);
        newPage->Next = nullptr;

#if ALOCATOR_TRACK_STATS
        ++m_numPages;
        m_numBlocks += m_blocksPerPage;
        m_numFreeBlocks += m_blocksPerPage;
#endif

#if ALOCATOR_FILL_MEMORY
        FillFreePage(newPage);
#endif

        // push new page
        newPage->Next = m_pageList;
        m_pageList = newPage;

        // link new free blocks
        BlockHeader* currBlock = Blocks(newPage);
        for (unsigned i = 1; i < m_blocksPerPage; ++i) {
            currBlock->Next = NextBlock(currBlock);
            currBlock = currBlock->Next;
        }
        currBlock->Next = nullptr; // last block

        // push new blocks
        m_freeList = Blocks(newPage);
    }

    // pop free block
    BlockHeader* freeBlock = m_freeList;
    m_freeList = m_freeList->Next;

#if ALOCATOR_TRACK_STATS
    --m_numFreeBlocks;
#endif

#if ALOCATOR_FILL_MEMORY
    FillAllocatedBlock(freeBlock);
#endif

    return freeBlock;
}

void FixedSizeAllocator::Free(void* p)
{
    // retrieve block header
    BlockHeader* block = reinterpret_cast<BlockHeader*>(p);

#if ALOCATOR_FILL_MEMORY
    FillFreeBlock(block);
#endif

    // push block
    block->Next = m_freeList;
    m_freeList = block;

#if ALOCATOR_TRACK_STATS
    ++m_numFreeBlocks;
#endif
}

void FixedSizeAllocator::FreeAll(void)
{
    // free all pages
    while (m_pageList) {
        PageHeader* currPage = m_pageList;
        m_pageList = m_pageList->Next;

        delete[] reinterpret_cast<char*>(currPage);
    }
    m_freeList = nullptr;

#if ALOCATOR_TRACK_STATS
    // re-init stats
    m_numPages = 0;
    m_numBlocks = 0;
    m_numFreeBlocks = 0;
#endif
}


void FixedSizeAllocator::FillFreePage(PageHeader* p)
{
    BlockHeader* currBlock = Blocks(p);
    char* const pageHeaderEnd = reinterpret_cast<char*>(p) + sizeof(PageHeader);
    std::memset(
        pageHeaderEnd,
        PATTERN_ALIGN,
        (uintptr_t)currBlock - (uintptr_t)pageHeaderEnd
    );

    for (unsigned i = 0; i < m_blocksPerPage; ++i) {
        FillFreeBlock(currBlock);
        currBlock = NextBlock(currBlock);
    }
}

void FixedSizeAllocator::FillFreeBlock(BlockHeader* p)
{
    std::memset(
        p,
        PATTERN_FREE,
        m_blockSize - m_blockAlignmentPadding
    );

    std::memset(
        reinterpret_cast<char*>(p) + m_blockSize - m_blockAlignmentPadding,
        PATTERN_ALIGN,
        m_blockAlignmentPadding
    );
}

void FixedSizeAllocator::FillAllocatedBlock(BlockHeader* p)
{
    std::memset(
        p,
        PATTERN_ALLOC,
        m_blockSize - m_blockAlignmentPadding
    );

    std::memset(
        reinterpret_cast<char*>(p) + m_blockSize - m_blockAlignmentPadding,
        PATTERN_ALIGN,
        m_blockAlignmentPadding
    );
}
