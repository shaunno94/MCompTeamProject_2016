#pragma once
#include "Rendering\GameObject.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\WheelObject.h"
#include "Rendering\ParticleSystem.h"
#include "Rendering\CubeEmitter.h"
#include "Application\constants.h"

class CarGameObject :
	public GameObject
{
public:
	CarGameObject(Vec3Physics& position, QuatPhysics& rotation, Material* material, unsigned int team, const std::string& name = "", unsigned int collisionMask = COL_CAR);
	~CarGameObject();

	bool isOnBlueTeam() { return m_team == BLUE_TEAM; }
protected:
	unsigned int m_team;
};

