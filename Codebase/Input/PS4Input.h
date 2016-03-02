#ifdef ORBIS
#pragma once

#include "pad.h"
#include "InputBase.h"

#define AXIS_INDEX_LEFTSTICK 0

enum AxisIndex
{
	LEFTSTICK = 0,
	RIGHTSTICK = 1,
	DPAD = 2
};

enum ButtonIndex
{
	BTN_TRIANGLE = 0,
	BTN_CIRCLE = 1,
	BTN_CROSS = 2,
	BTN_SQUARE = 3,
	BTN_L2 = 4,
	BTN_R2 = 5,
	BTN_L1 = 6,
	BTN_R1 = 7,
	BTN_L3 = 8,
	BTN_R3 = 9
};

class PS4Input : public InputBase
{
public:
	PS4Input();
	~PS4Input();

	void Poll();

	static PS4Input* getPS4Input()	{ return input; }

protected:
	void InitController();

	int32_t padHandle;
	ScePadControllerInformation padInfo;
	static PS4Input* input;
};
#endif
