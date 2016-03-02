#pragma once
#include "Rendering\GameObject.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\WheelObject.h"

class CarGameObject :
	public GameObject
{
public:
	CarGameObject(Vec3Physics& position, QuatPhysics& rotation, Material* material, const std::string& name = "");
	~CarGameObject();
};

