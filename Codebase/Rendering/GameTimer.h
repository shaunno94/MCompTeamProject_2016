#include "Helpers/DeltaTimer.h"

class GameTimer
{
public:
	GameTimer()	{ timer = new DeltaTimer<float>(); elapsedMS = timer->Age(1000.0f);}
		
	DeltaTimer<float>* GetTimer() { return timer; } 

private:
	DeltaTimer<float>*	timer;
	float				elapsedMS;
};