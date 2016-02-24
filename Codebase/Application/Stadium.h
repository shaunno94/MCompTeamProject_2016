#pragma once
#include "Rendering/GameObject.h"
#include "Rendering/Material.h"
#include <string>
#include "Rendering/ModelLoader.h"

class Stadium : public GameObject
{
public:
	Stadium(Material* material, Material* wallMaterial, const std::string& name = "");
	~Stadium();

private:
	void CreateCollisionWalls();
	void CreatePlane(std::vector<btConvexHullShape*> &collectionVector, Vec3Physics start, Vec3Physics end);

	std::vector<btConvexHullShape*> m_collisionWalls;

	Material* m_wallMaterial;
	Texture* m_netTexture;
};

