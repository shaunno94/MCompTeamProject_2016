#include "Timer.h"


#ifdef _WIN32
/// <summary>
/// Gets the computers time measurement frequency.
/// </summary>
/// <returns>Time measurement frequency.</returns>
static double GetQueryPerformancePeriod() {
	LARGE_INTEGER result;
	QueryPerformanceFrequency(&result);
	return 1.0 / static_cast<double>(result.QuadPart);
}
const double Timer::PERIOD = GetQueryPerformancePeriod();
#endif
