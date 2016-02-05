#include "Scene.h"


Scene::Scene()
{
	cam = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));
}


Scene::~Scene()
{
	for (auto obj : transparentObjects)
	{
		delete obj;
	}
	for (auto obj : opaqueObjects)
	{
		delete obj;
	}
	delete cam;
	transparentObjects.clear();
	opaqueObjects.clear();
}

void Scene::addGameObject(GameObject* obj, bool transparent)
{
	if (transparent)
	{
		transparentObjects.push_back(obj);
	}
	else
	{
		opaqueObjects.push_back(obj);
	}
}