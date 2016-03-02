#pragma once
#include "GameObject.h"
class WheelObject :
	public GameObject
{
public:
	WheelObject(const std::string& name = "");
	~WheelObject();

	void WheelObject::OnUpdateObject(float dt) override;
};

