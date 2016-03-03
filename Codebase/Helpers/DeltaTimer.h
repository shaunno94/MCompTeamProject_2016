#pragma once
#include "Timer.h"

template <typename T = float>
/// @ingroup Helpers
/// <summary>
/// Class for measuring time intervals.
/// </summary>
class DeltaTimer : public Timer
{
	static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "DeltaTime template only supports float or double types.");

protected:
	/// <summary>
	/// Age value for the last time measurement.
	/// </summary>
	T lastAge;

public:
	/// <summary>
	/// Default class constructor.
	/// </summary>
	inline DeltaTimer(): lastAge(0.0) {}

	/// <summary>
	/// Peek the value for current time interval without ending it.
	/// </summary>
	/// <param name="timeResolution">Time interval resolution; 1 for seconds (default), 1000 for milliseconds</param>
	/// <returns>Time interval.</returns>
	inline T Peek(float timeResolution = 1.0f);

	/// <summary>
	/// Get the value for current time interval and start a new one.
	/// </summary>
	/// <param name="timeResolution">Time interval resolution; 1 for seconds (default), 1000 for milliseconds</param>
	/// <returns>Time interval.</returns>
	inline T Get(float timeResolution = 1.0f);

	/// <summary>
	/// Peek the value for current time interval of at least a given amount, without ending it.
	/// </summary>
	/// <param name="minDelta">Minimal time interval to return.</param>
	/// <param name="timeResolution">Time interval resolution; 1 for seconds (default), 1000 for milliseconds.</param>
	/// <returns>Time interval if it is more or equal to minDelta or 0 otherwise.</returns>
	inline T PeekMin(float minDelta, float timeResolution = 1.0f);

	/// <summary>
	/// Get the value for current time interval of at least a given amount and start a new one.
	/// </summary>
	/// <param name="minDelta">Minimal time interval to return.</param>
	/// <param name="timeResolution">Time interval resolution; 1 for seconds (default), 1000 for milliseconds.</param>
	/// <returns>Time interval if it is more or equal to minDelta or 0 otherwise.</returns>
	inline T GetMin(float minDelta, float timeResolution = 1.0f);
};

template <typename T>
T DeltaTimer<T>::Peek(float timeResolution)
{
	return (T) ((Age(1.0f) - lastAge) * timeResolution);
}


template <typename T>
T DeltaTimer<T>::Get(float timeResolution)
{
	float newAge = Age(1.0f);
	float result = newAge - lastAge;
	lastAge	= newAge;
	return (T) (result * timeResolution);
}


template <typename T>
T DeltaTimer<T>::PeekMin(float minDelta, float timeResolution)
{
	float scaledResult = (Age(1.0f) - lastAge) * timeResolution;
	return (scaledResult >= minDelta) * scaledResult;
}


template <typename T>
T DeltaTimer<T>::GetMin(float minDelta, float timeResolution)
{
	T newAge = Age(1.0f);
	T scaledResult = (newAge - lastAge) * timeResolution;
	if(scaledResult >= minDelta)
	{
		lastAge	= newAge;
		return scaledResult;
	}
	return 0.0f;
}
