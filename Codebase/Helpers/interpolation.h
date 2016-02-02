#pragma once

template<typename T>
static inline float Lerp(const T& start, const T& end, float progress)
{
	return (end - start) * progress + start ;
};

template<typename T>
//http://en.wikipedia.org/wiki/Smooth
static inline float Smooth(const T& start, const T& end, float progress)
{
	return Lerp(start, end, progress * progress * (3 - 2 * progress));
}

template<typename T>
static inline float Smoother(const T& start, const T& end, float progress)
{
	return Lerp(start, end, progress * progress * progress * (progress * (progress * 6 - 15) + 10));
}