#pragma once
#include <unordered_map>
#include "PickupGameObject.h"

class PickupManager
{
public:
	PickupManager(Material* material);
	~PickupManager();

	PickupGameObject* FindPickup(int uid);
	const std::unordered_map<int, PickupGameObject*>* GetPickups();

protected:
	std::unordered_map<int, PickupGameObject*> m_pickups;
	Mesh* m_boostMesh;
};

