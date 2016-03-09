#pragma once

struct AXIS {
	float x;
	float y;

	AXIS() {
		x = 0.0f;
		y = 0.0f;
	}
};

#define MAX_AXIS 8
#define MAX_BUTTONS 127

class InputBase
{
public:
	InputBase();
	~InputBase();

	void ResetInput();

	AXIS	GetAxis(unsigned int i);
	float	GetButton(unsigned int i);
	bool	GetButtonDown(unsigned int i);
	bool	GetButtonTriggered(unsigned int i);
	bool	GetButtonHeld(unsigned int i);

protected:

	AXIS	axis[MAX_AXIS];
	float   buttons[MAX_BUTTONS];
	float   buttonsHeld[MAX_BUTTONS];
	void	UpdateHolds();
};


