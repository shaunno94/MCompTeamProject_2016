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
	BTN_CROSS = 1,
	BTN_CIRCLE = 2,
	BTN_SQUARE = 3,
	L2 = 4,
	R2 = 5,
	L1 = 6,
	R1 = 7,
	L3 = 8,
	R3 = 9
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
