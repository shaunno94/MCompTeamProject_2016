#pragma once

#include <cstdint>

//TODO: add hardware supported commands (https://en.wikipedia.org/wiki/Find_first_set)

//DeBruijn
inline int lowestBitPosition(uint32_t x)
{
	static const int LowestBitPositionDeBruijnLookup32[32] =
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	return LowestBitPositionDeBruijnLookup32[static_cast<uint32_t>((x & (-1 * x)) * 0x077CB531U) >> 27];
}

//DeBruijn
//returns 0 when x is 0
inline int lowestBitPosition(uint64_t x)
{
	static const int LowestBitPositionDeBruijnLookup64[64] =
	{
		0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
		62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
		63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
		46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
	};
	return LowestBitPositionDeBruijnLookup64[static_cast<uint64_t>((x & (-1 * x)) * 0x03F79D71B4CB0A89U) >> 58];
}

//"Hamming weight" or "population count"
int popcount(uint32_t x)
{
	static const uint32_t m1_32 = 0x55555555; //binary: 0101...
	static const uint32_t m2_32 = 0x33333333; //binary: 00110011..
	static const uint32_t m4_32 = 0x0f0f0f0f; //binary:  4 zeros,  4 ones ...
	static const uint32_t h01_32 = 0x01010101; //the sum of 256 to the power of 0,1,2,3...

	x = x - ((x >> 1) & m1_32);
	x = (x & m2_32) + ((x >> 2) & m2_32);
	x = (x + (x >> 4)) & m4_32;
	return (x * h01_32) >> 24;
}

//"Hamming weight" or "population count"
int popcount(uint64_t x)
{
	static const uint64_t m1_64 = 0x5555555555555555; //binary: 0101...
	static const uint64_t m2_64 = 0x3333333333333333; //binary: 00110011..
	static const uint64_t m4_64 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
	static const uint64_t h01_64 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

	x -= (x >> 1) & m1_64;             //put count of each 2 bits into those 2 bits
	x = (x & m2_64) + ((x >> 2) & m2_64); //put count of each 4 bits into those 4 bits
	x = (x + (x >> 4)) & m4_64;        //put count of each 8 bits into those 8 bits
	return (x * h01_64) >> 56; //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}
