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

//Recursively add objects to node lists for rendering - check for transparency
void Scene::addGameObject(GameObject* obj)
{
	for (auto child : obj->GetChildren())
	{
		addGameObject(child);
	}
	obj->m_RenderComponent->m_Material->hasTranslucency ? transparentObjects.push_back(obj) : opaqueObjects.push_back(obj);
}

void Scene::UpdateNodeLists(float dt)
{
	Vec3Graphics pos, dir;
	Vec3Graphics camPos = cam->GetPosition();

	//For opaque and translucent objects update and compute (sqr) distance between object and camera.
	for (unsigned int i = 0; i < transparentObjects.size(); ++i)
	{
		transparentObjects[i]->OnUpdateObject(dt);
		pos = transparentObjects[i]->GetWorldTransform().GetTranslation();
		dir = pos - camPos;
		transparentObjects[i]->m_CamDist = dir.Dot(dir);
	}
	for (unsigned int i = 0; i < opaqueObjects.size(); ++i)
	{
		opaqueObjects[i]->OnUpdateObject(dt);
		pos = opaqueObjects[i]->GetWorldTransform().GetTranslation();
		dir = pos - camPos;
		opaqueObjects[i]->m_CamDist = dir.Dot(dir);
	}

	//Sort lists using insertion sort - O(N) best case when list is almost ordered which should be the case most of the time.
	InsertionSort(transparentObjects.begin(), transparentObjects.end(), Scene::CompareByCameraDistanceInv);
	InsertionSort(opaqueObjects.begin(), opaqueObjects.end(), Scene::CompareByCameraDistance);
}