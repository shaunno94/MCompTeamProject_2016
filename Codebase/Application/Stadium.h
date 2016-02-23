#pragma once
#include "Rendering/GameObject.h"
#include "Rendering/Material.h"
#include <string>
#include "Rendering/ModelLoader.h"

class Stadium : public GameObject
{
public:
	Stadium(Material* material, const std::string& name = "");
	~Stadium();

private:
	void CreateCollisionWalls();
	void CreateWall(Vec2Physics start, Vec2Physics end);

	std::vector<btConvexHullShape*> m_collisionWalls;
};

