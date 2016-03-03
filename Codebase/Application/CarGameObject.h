#pragma once
#include "Rendering\GameObject.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\WheelObject.h"

class CarGameObject :
	public GameObject
{
public:
	CarGameObject(const Vec3Physics& position, const QuatPhysics& rotation, Material* material, const std::string name = "", unsigned int collisionMask = COL_CAR);
	~CarGameObject();
};

