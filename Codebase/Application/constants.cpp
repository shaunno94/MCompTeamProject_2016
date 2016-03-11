#include "constants.h"


#ifndef ORBIS
NetworkGameData NetworkGameData::Instance = {
	Vec3Physics(200, 2, 0),
	Vec3Physics(300, 2, 30),
	Vec3Physics(300, 2, -30),
	Vec3Physics(-200, 2, 0),
	Vec3Physics(-300, 2, 30),
	Vec3Physics(-300, 2, -30),
	QuatPhysics(0, 0, 0, 1),
	QuatPhysics(0, 0, 0, 1),
	QuatPhysics(0, 0, 0, 1),
	QuatPhysics(0, 0, 0, 1),
	QuatPhysics(0, 0, 0, 1),
	QuatPhysics(0, 0, 0, 1),
	nullptr,
	nullptr
};
#endif