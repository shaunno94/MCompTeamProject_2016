#pragma once

class UIControllerComponent
{
public:
	virtual ~UIControllerComponent(){};

	virtual void SelectNext() = 0;
	virtual void SelectPrevious() = 0;
	virtual void Submit() = 0;
};