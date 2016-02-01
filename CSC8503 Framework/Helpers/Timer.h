#pragma once

#include "common.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <chrono>
#endif


#ifdef _WIN32
typedef LARGE_INTEGER timestamp;
#else
typedef std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
#endif


class Timer {
 protected:
	timestamp start;

#ifdef _WIN32
	/// <summary>
	/// Computer time measurement frequency for representing <see cref="Performance::Timer::record"/> data in terms of real time.
	/// </summary>
	static const double PERIOD;
#endif

 public:

	/// <summary>
	/// Gets the value of current point in time.
	/// </summary>
	/// <returns>Timestamp of current time.</returns>
	inline static timestamp Now() {
#ifdef _WIN32
		timestamp temp;
		QueryPerformanceCounter(&temp);
		return temp;
#else
		return std::chrono::high_resolution_clock::now();
#endif
	}

	/// <summary>
	/// Isolated functionality for determining the duration between timestamps, depending on platform timestamp definition.
	/// </summary>
	/// <param name="start">Timestamp of the start of the period.</param>
	/// <param name="end">Timestamp of the end of the period.</param>
	/// <param name="timeResolution">Time period resolution, 1 = seconds and 1000000 = microseconds.</param>
	/// <returns>Duration of the time period</returns>
	inline static float Duration(timestamp start, timestamp end, float timeResolution) {
#ifdef _WIN32
		return static_cast<float>((end.QuadPart - start.QuadPart) * timeResolution * Timer::PERIOD);
#else
		return static_cast<float>(return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() * timeResolution);
#endif
	}

	/// <summary>
	/// Isolated functionality for determining the duration between timestamps, depending on platform timestamp definition.
	/// </summary>
	/// <param name="start">Timestamp of the start of the period.</param>
	/// <param name="end">Timestamp of the end of the period.</param>
	/// <param name="timeResolution">Time period resolution, 1 = seconds and 1000000 = microseconds.</param>
	/// <returns>Duration of the time period</returns>
	inline static double Duration(timestamp start, timestamp end, double timeResolution = 1.0) {
#ifdef _WIN32
		return (end.QuadPart - start.QuadPart) * timeResolution * Timer::PERIOD;
#else
		return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() * timeResolution;
#endif
	}


	inline Timer(): start(Timer::Now()) {}

	inline float Age(float timeResolution) {
		return Timer::Duration(start, Timer::Now(), timeResolution);
	}
	inline double Age(double timeResolution = 1.0) {
		return Timer::Duration(start, Timer::Now(), timeResolution);
	}
};
