#pragma once

#include <string>
#include <algorithm>
#include <smmintrin.h>

/** @defgroup Helpers Helpers
*  Collection of helpful functionality.
*  @{
*/

/*
MEM_ALIGN
struct {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		__m128 mmvalue;
	};
};
*/

/// <summary>
/// Macro for calling methods with function pointers.
/// </summary>
#define CALL_MEMBER_FN(instance, ptrToMemberFn)  ((instance).*(ptrToMemberFn))

template <typename T>
inline T Squared(T v)
{
	return v * v;
}

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


template <typename T>
/// <summary>
/// Helper function for clamping values.
/// </summary>
/// <param name="in">Input.</param>
/// <param name="min">Min value.</param>
/// <param name="max">Max value.</param>
/// <returns>Clamped value.</returns>
T ClampValues(T in, T min, T max)
{
	return std::min(std::max(in, min), max);
}

/** @} */