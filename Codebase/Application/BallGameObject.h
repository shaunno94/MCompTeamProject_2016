#pragma once
#include "Rendering\GameObject.h"
#include "Rendering\Material.h"

class BallGameObject : public GameObject
{
public:
	BallGameObject(const std::string& name, Material* material);
	~BallGameObject();
};

