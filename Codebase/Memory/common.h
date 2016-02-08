#pragma once 


#define MEM_ALIGNMENT 16

#define MEM_ALIGN __declspec(align(MEM_ALIGNMENT))

#define MEM_ALIGN_NEW \
	inline void* operator new	(size_t size) { return _aligned_malloc(size, MEM_ALIGNMENT); } \
	inline void* operator new[]	(size_t size) { return _aligned_malloc(size, MEM_ALIGNMENT); }

#define MEM_ALIGN_DELETE \
	inline void operator delete		(void* p) { _aligned_free(p); } \
	inline void operator delete[]	(void* p) { _aligned_free(p); }

#define MEM_ALIGN_NEW_DELETE \
	MEM_ALIGN_NEW \
	MEM_ALIGN_DELETE
