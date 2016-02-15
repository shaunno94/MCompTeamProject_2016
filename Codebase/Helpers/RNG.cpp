#include "RNG.h"

//manual switch to use random_device for seeding
#define SUPPORT_STD_RANDOM_DEVICE 1

#if !SUPPORT_STD_RANDOM_DEVICE
#include <chrono>
#endif


RNG32 RNG32::Rand = RNG32(RNG32::RandSeed());
RNG64 RNG64::Rand = RNG64(RNG64::RandSeed());

uint32_t RNG32::RandSeed()
{
#if SUPPORT_STD_RANDOM_DEVICE
	std::random_device randDevice;
	return randDevice();
#else
	//crushto32 function from https://gist.github.com/imneme/540829265469e673d045
	uint64_t result = uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	result *= 0xbc2ad017d719504d;
	return uint32_t(result ^ (result >> 32));
#endif
}


uint64_t RNG64::RandSeed()
{
#if SUPPORT_STD_RANDOM_DEVICE
	std::random_device randDevice;
	uint64_t value = randDevice();
	return (value << 32) | randDevice();
#else
	return uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());
#endif
}