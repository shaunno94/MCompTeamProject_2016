#pragma once
#include "Rendering\GameObject.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\WheelObject.h"
#include "Rendering\ParticleSystem.h"
#include "Rendering\CubeEmitter.h"

class CarGameObject :
	public GameObject
{
public:
	CarGameObject(Vec3Physics& position, QuatPhysics& rotation, Material* material, const std::string& name = "");
	~CarGameObject();
};

