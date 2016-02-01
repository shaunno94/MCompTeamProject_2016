#pragma once
#include "Timer.h"

template <typename T>
class DeltaTimer : public Timer {
	static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "DeltaTime template only supports float or double types.");

 protected:
	T lastAge;

 public:
	inline DeltaTimer(): lastAge(0.0) {}

	inline T Peek(float timeResolution = 1.0f);

	inline T Get(float timeResolution = 1.0f);

	inline T PeekMin(float minDelta, float timeResolution = 1.0f);

	inline T GetMin(float minDelta, float timeResolution = 1.0f);
};

template <>
inline float DeltaTimer<float>::Peek(float timeResolution) { return (Age(1.0f) - lastAge) * timeResolution; }

template <>
inline double DeltaTimer<double>::Peek(float timeResolution) { return (Age(1.0) - lastAge) * timeResolution; }


template <>
inline float DeltaTimer<float>::Get(float timeResolution) {
	float newAge = Age(1.0f);
	float result = newAge - lastAge;
	lastAge	= newAge;
	return result * timeResolution;
}

template <>
inline double DeltaTimer<double>::Get(float timeResolution) {
	double newAge = Age(1.0);
	double result = newAge - lastAge;
	lastAge	= newAge;
	return result * timeResolution;
}


template <>
inline float DeltaTimer<float>::PeekMin(float minDelta, float timeResolution) {
	float scaledResult = (Age(1.0f) - lastAge) * timeResolution;
	return (scaledResult >= minDelta) * scaledResult;
}

template <>
inline double DeltaTimer<double>::PeekMin(float minDelta, float timeResolution) {
	double scaledResult = (Age(1.0) - lastAge) * timeResolution;
	return (scaledResult >= minDelta) * scaledResult;
}


template <>
inline float DeltaTimer<float>::GetMin(float minDelta, float timeResolution) {
	float newAge = Age(1.0f);
	float result = newAge - lastAge;
	float scaledResult = result * timeResolution;
	if(scaledResult >= minDelta) {
		lastAge	= newAge;
		return scaledResult;
	}
	return 0.0f;
}

template <>
inline double DeltaTimer<double>::GetMin(float minDelta, float timeResolution) {
	double newAge = Age(1.0);
	double result = newAge - lastAge;
	double scaledResult = result * timeResolution;
	if(scaledResult >= minDelta) {
		lastAge	= newAge;
		return scaledResult;
	}
	return 0.0;
}