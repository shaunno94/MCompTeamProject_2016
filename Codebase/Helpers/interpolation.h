#pragma once

/** @ingroup Helpers
*  @{
*/

//template<typename T>
/// <summary>
/// Linear interpolation.
/// </summary>
/// <param name="start">Starting value.</param>
/// <param name="end">End value.</param>
/// <param name="progress">Weighting from 0 to 1 for interpolation.</param>
/// <returns>Interpolated value.</returns>
//static inline float Lerp(const T& start, const T& end, float progress)
//{
//	return (end - start) * progress + start ;
//};

template<typename T>
/// <summary>
/// Smoothed interpolation.
/// </summary>
/// <param name="start">Starting value.</param>
/// <param name="end">End value.</param>
/// <param name="progress">Weighting from 0 to 1 for interpolation.</param>
/// <returns>Interpolated value.</returns>
static inline float Smooth(const T& start, const T& end, float progress)
{
	return Lerp(start, end, progress * progress * (3 - 2 * progress));
}

template<typename T>
/// <summary>
/// Smoothed interpolation with more shallow angles on the edges of the interpolation curve.
/// </summary>
/// <param name="start">Starting value.</param>
/// <param name="end">End value.</param>
/// <param name="progress">Weighting from 0 to 1 for interpolation.</param>
/// <returns>Interpolated value.</returns>
static inline float Smoother(const T& start, const T& end, float progress)
{
	return Lerp(start, end, progress * progress * progress * (progress * (progress * 6 - 15) + 10));
}

/** @} */