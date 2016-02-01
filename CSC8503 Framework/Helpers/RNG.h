#pragma once

#include <random>
#include <cstdint>

/// <summary>
/// Random number generator for 32bit numbers.
/// </summary>
/// <remarks>
/// Classes RNG32 and RNG64 can be instantiated for thread safety,
/// but a static instance is provided for easy use in a single thread application (<see cref="RNG32::Rand"/> or <see cref="RNG64::Rand"/>).
/// </remarks>
/// <remarks>
/// This class uses Mersenne Twister number generator engine that produces deterministic results.
/// The same seed can be used for several instances to produce identical number sequences and
/// <see cref="RNG32::discard"/> can be used to synch up instances that have generated different amount of numbers.
/// </remarks>
///
/// <example>
/// Usage of the provided static instance, where min and max defines a range for generating numbers.
/// <code>
/// RNG32::Rand(min, max);
/// </code>
/// </example>
///
/// <example>
/// Usage of instantiated class, where min and max defines a range for generating numbers.
/// <code>
/// RNG32 rand(RNG32::RandSeed());
/// rand(min, max);
/// </code>
/// </example>
class RNG32 {
 private:
	/// <summary>
	/// Mersenne twister number generator engine.
	/// </summary>
	std::mt19937 engine;

	/// <summary>
	/// Counter for keeping track of how many numbers have been generated.
	/// </summary>
	uint64_t generationCount;

 public:
	/// <summary>
	/// Default constructor that seeds the RNG engine with a default seed (usually 5489U).
	/// </summary>
	inline RNG32() : generationCount(0) {}

	/// <summary>
	/// Constructor with a seed for the RNG engine.
	/// </summary>
	/// <param name="seed">Seed for the RNG engine.</param>
	inline RNG32(uint32_t seed) : generationCount(0), engine(seed) {}

	/// <summary>
	/// Constructor with a seed for the RNG engine and a starting position in the random number sequence.
	/// </summary>
	/// <param name="seed">Seed for the RNG engine.</param>
	/// <param name="skip">The amount of numbers to skip for the RNG generator.</param>
	inline RNG32(uint32_t seed, uint64_t skip) : generationCount(skip), engine(seed) { engine.discard(skip); }

	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline float operator()(float min, float max) {
		++generationCount;
		std::uniform_real_distribution<float> realDist(min, max);
		return realDist(engine);
	}

	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline int32_t operator()(int32_t min, int32_t max) {
		++generationCount;
		std::uniform_int_distribution<int32_t> intDist(min, max);
		return intDist(engine);
	}

	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline uint32_t operator()(uint32_t min, uint32_t max) {
		++generationCount;
		std::uniform_int_distribution<uint32_t> uintDist(min, max);
		return uintDist(engine);
	}

	/// <summary>
	/// Get the indicator for how many generations the RNG engine has went through.
	/// </summary>
	/// <returns>Number of generations.</returns>
	inline uint64_t GetNumbersGenerated() const {
		return generationCount;
	}

	/// <summary>
	/// Discards the given amount of numbers in the random number sequence.
	/// </summary>
	/// <param name="steps">Amount of numbers to discard.</param>
	inline void Discard(uint64_t steps) {
		engine.discard(steps);
		generationCount += steps;
	}

	/// <summary>
	/// Produces a random seed for seeding an RNG engine, using a non-deterministic RNG source or a timestamp.
	/// </summary>
	/// <returns>Random seed.</returns>
	static uint32_t RandSeed();

	/// <summary>
	/// Static instance of a RNG32 class with a random seed for easy use.
	/// </summary>
	/// <remarks>
	/// This class is not thread safe, so it is recommended for threads to have their own personal instance for RNG32.
	/// </remarks>
	static RNG32 Rand;
};

/// <summary>
/// Random number generator for 64bit numbers.
/// </summary>
/// <remarks>
/// Classes RNG32 and RNG64 can be instantiated for thread safety,
/// but a static instance is provided for easy use in a single thread application (<see cref="RNG32::Rand"/> or <see cref="RNG64::Rand"/>).
/// </remarks>
/// <remarks>
/// This class uses Mersenne Twister number generator engine that produces deterministic results.
/// The same seed can be used for several instances to produce identical number sequences and
/// <see cref="RNG64::discard"/> can be used to synch up instances that have generated different amount of numbers.
/// </remarks>
///
/// <example>
/// Usage of the provided static instance, where min and max defines a range for generating numbers.
/// <code>
/// RNG64::Rand(min, max);
/// </code>
/// </example>
///
/// <example>
/// Usage of instantiated class, where min and max defines a range for generating numbers.
/// <code>
/// RNG64 rand(RNG64::RandSeed());
/// rand(min, max);
/// </code>
/// </example>
class RNG64 {
 private:
	/// <summary>
	/// Mersenne twister number generator engine.
	/// </summary>
	std::mt19937_64 engine;

	/// <summary>
	/// Counter for keeping track of how many numbers have been generated.
	/// </summary>
	uint64_t generationCount;

 public:
	/// <summary>
	/// Default constructor that seeds the RNG engine with a default seed (usually 5489U).
	/// </summary>
	RNG64() : generationCount(0) {}

	/// <summary>
	/// Constructor with a seed for the RNG engine.
	/// </summary>
	/// <param name="seed">Seed for the RNG engine.</param>
	RNG64(uint64_t seed) : generationCount(0), engine(seed) {}

	/// <summary>
	/// Constructor with a seed for the RNG engine and a starting position in the random number sequence.
	/// </summary>
	/// <param name="seed">Seed for the RNG engine.</param>
	/// <param name="skip">The amount of numbers to skip for the RNG generator.</param>
	RNG64(uint64_t seed, uint64_t skip) : generationCount(skip), engine(seed) { engine.discard(skip); }


	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline double operator()(double min, double max) {
		++generationCount;
		std::uniform_real_distribution<double> realDist(min, max);
		return realDist(engine);
	}

	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline int64_t operator()(int64_t min, int64_t max) {
		++generationCount;
		std::uniform_int_distribution<int64_t> intDist(min, max);
		return intDist(engine);
	}

	/// <summary>
	/// Generate next random number in range.
	/// </summary>
	/// <param name="min">The starting value of the generating number range (inclusive).</param>
	/// <param name="max">The ending value of the generating number range (inclusive).</param>
	/// <returns>Next random number in range.</returns>
	inline uint64_t operator()(uint64_t min, uint64_t max) {
		++generationCount;
		std::uniform_int_distribution<uint64_t> uintDist(min, max);
		return uintDist(engine);
	}

	/// <summary>
	/// Get the indicator for how many generations the RNG engine has went through.
	/// </summary>
	/// <returns>Number of generations.</returns>
	inline uint64_t GetNumbersGenerated() const {
		return generationCount;
	}

	/// <summary>
	/// Discards the given amount of numbers in the random number sequence.
	/// </summary>
	/// <param name="steps">Amount of numbers to discard.</param>
	inline void Discard(uint64_t steps) {
		engine.discard(steps);
		generationCount += steps;
	}

	/// <summary>
	/// Produces a random seed for seeding an RNG engine, using a non-deterministic RNG source or a timestamp.
	/// </summary>
	/// <returns>Random seed.</returns>
	static uint64_t RandSeed();

	/// <summary>
	/// Static instance of a RNG64 class with a random seed for easy use.
	/// </summary>
	/// <remarks>
	/// This class is not thread safe, so it is recommended for threads to have their own personal instance for RNG64.
	/// </remarks>
	static RNG64 Rand;
};
