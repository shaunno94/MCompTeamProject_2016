#pragma once
#include "Rendering/GameObject.h"
#include "Rendering/Material.h"
#include <string>
#include "Rendering/ModelLoader.h"
#include "Rendering/ExtendedMaterial.h"

class Stadium : public GameObject
{
public:
	Stadium(Material* material, Material* wallMaterial, ExtendedMaterial* redPostMaterial, ExtendedMaterial* bluePostMaterial, const std::string& name = "");
	~Stadium();

private:
	void CreateCollisionWalls();
	void CreatePlane(std::vector<btConvexHullShape*> &collectionVector, Vec3Physics start, Vec3Physics end, bool first = false, bool roof = false);

	std::vector<btConvexHullShape*> m_collisionWalls;

	Material* m_wallMaterial;
	Texture* m_netTexture;

	ExtendedMaterial* m_redPostMaterial;
	ExtendedMaterial* m_bluePostMaterial;
	Texture* m_postTexture;

	Mesh* m_postMesh;
	Mesh* m_stadiumMesh;
	std::vector<Mesh*> m_walls;
};

