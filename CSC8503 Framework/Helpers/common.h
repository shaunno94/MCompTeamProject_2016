/******************************************************************************
Author:Rich Davison
Description: Some random variables and functions, for lack of a better place
to put them.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#define WEEK_2_CODE
#define USE_MD5MESH
//#define WEEK_3_CODE

#include <xmmintrin.h>
#include <string>
#include <algorithm>
#include <functional>
#include <vector>
#include <malloc.h>

/// <summary>
/// Macro for calling methods with function pointers.
/// </summary>
#define CALL_MEMBER_FN(instance, ptrToMemberFn)  ((instance).*(ptrToMemberFn))

template <typename T>
inline T Squared(T v) { return v * v; }

typedef unsigned int uint;
typedef unsigned char uchar;

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


//MEM_ALIGN?
struct __m128Converter {
	union {
		struct {
			float x, y, z, w;
		};
		__m128 mmvalue;
	};
};


/// <summary>
/// Empty string.
/// </summary>
static const std::string EMPTY_STR = "";
/// <summary>
/// Empty string.
/// </summary>
static const char* const EMPTY_CHAR_ARRAY = EMPTY_STR.c_str();

/// <summary>
/// Platform specific line separation string.
/// </summary>
static const std::string LINE_SEPARATOR_STR =
#ifdef _WIN32
  "\r\n";
#elif __APPLE__
  "\r";
#else
  "\n";
#endif

/// <summary>
/// Platform specific line separation character array.
/// </summary>
static const char* const LINE_SEPARATOR =	LINE_SEPARATOR_STR.c_str();


template<typename T>
void SwapErase(std::vector<T>& v, size_t inx) {
	std::swap(v[inx], v.back());
	v.pop_back();
}

template<typename T>
void SwapErase(std::vector<T>& v, const T& val) {
	std::swap(val, v.back());
	v.pop_back();
}

template<typename T>
bool FindSwapErase(std::vector<T>& v, const T& val) {
	std::vector<T>::iterator it = std::find(v.begin(), v.end(), val);
	if (it != v.end()) {
		std::swap(*it, v.back());
		v.pop_back();
		return true;
	}
	return false;
}

template <typename T>
T Clamp(T in, T low, T high) {
	return std::min(std::max(in, low), high);
}

template <class FwdIt, class Compare>
void InsertionSort(FwdIt first, FwdIt last, Compare cmp) {
	if (first != last) {
		FwdIt originalStart = first;
		for (++first; first != last; ++first) {
			FwdIt current = first;
			FwdIt previous = first - 1;
			while (current != originalStart && cmp(*current, *previous)) {
				std::iter_swap(current, previous);
				--current;
			}
		}
	}
}

template <class FwdIt>
void InsertionSort(FwdIt first, FwdIt last) {
	InsertionSort(first, last, std::less<typename std::iterator_traits<FwdIt>::value_type>());
}

#include "degrees.h"