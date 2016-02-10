#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <chrono>
#endif


/// @ingroup Helpers
/// <summary>
/// Base class for time representation.
/// </summary>
class Timer
{
public:

#ifdef _WIN32
	/// <summary>
	/// Computer time measurement frequency for representing <see cref="Performance::Timer::record"/> data in terms of real time.
	/// </summary>
	static const double PERIOD;
#endif

#ifdef _WIN32
	typedef LARGE_INTEGER timestamp;
#else
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
#endif

	/// <summary>
	/// Gets the value of current point in time.
	/// </summary>
	/// <returns>Timestamp of current time.</returns>
	inline static timestamp Now()
	{
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
	inline static double Duration(timestamp start, timestamp end, float timeResolution = 1.0f)
	{
#ifdef _WIN32
		return (end.QuadPart - start.QuadPart) * timeResolution * Timer::PERIOD;
#else
		return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() * timeResolution;
#endif
	}

	/// <summary>
	/// Default class constructor.
	/// </summary>
	inline Timer(): m_Start(Timer::Now()) {}

	/// <summary>
	/// Find out the instance age.
	/// </summary>
	/// <param name="timeResolution">Time resolution; 1 for seconds (default), 1000 for milliseconds.</param>
	/// <returns>Time interval since object instantiation.</returns>
	inline double Age(float timeResolution = 1.0f)
	{
		return Timer::Duration(m_Start, Timer::Now(), timeResolution);
	}

protected:
	/// <summary>
	/// Timestamp of the instantiation.
	/// </summary>
	timestamp m_Start;

};
