#pragma once
#include "InputBase.h"
#include<windows.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC			((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE			((USHORT) 0x02)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD		((USHORT) 0x06)
#endif

class WindowsInput :
	public InputBase
{
public:
	WindowsInput();
	~WindowsInput();
};

