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
	for (auto &obj : opaqueObjects)
	{
		delete obj;		
	}
	for (auto obj : ghostObjects)
	{
		delete obj;
	}

	delete cam;
	/*if (playerController)
		delete playerController;*/
	transparentObjects.clear();
	opaqueObjects.clear();
	ghostObjects.clear();
}

GameObject* Scene::findGameObject(const std::string& objectName)
{
	for (auto obj : ghostObjects) {
		if (obj->m_Name == objectName) {
			return obj;
		}
	}
	for (auto obj : opaqueObjects) {
		if (obj->m_Name == objectName) {
			return obj;
		}
	}
	for (auto obj : transparentObjects) {
		if (obj->m_Name == objectName) {
			return obj;
		}
	}
	return nullptr;

}

//Recursively add objects to node lists for rendering - check for transparency
void Scene::addGameObject(GameObject* obj)
{
	for (auto child : obj->GetChildren())
	{
		addGameObject(child);
	}
	if (obj->m_RenderComponent)
		obj->m_RenderComponent->m_Material->hasTranslucency ? transparentObjects.push_back(obj) : opaqueObjects.push_back(obj);
	else 
		ghostObjects.push_back(obj);
}

void Scene::UpdateNodeLists(float dt, Frustum& frustum, Vec3Graphics camPos)
//TODO - separate Frustum check from OnUpdateObject
{
	Vec3Graphics pos, dir;

	//For opaque and translucent objects update and compute (sqr) distance between object and camera.
	for (unsigned int i = 0; i < transparentObjects.size(); ++i)
	{
		transparentObjects[i]->OnUpdateObject(dt);

	}
	for (unsigned int i = 0; i < opaqueObjects.size(); ++i)
	{
		opaqueObjects[i]->OnUpdateObject(dt);

	}
	for (unsigned int i = 0; i < ghostObjects.size(); ++i)
	{
		ghostObjects[i]->OnUpdateObject(dt);

	}

	UpdateFrustumCulling(frustum, camPos);

	//Sort lists using insertion sort - O(N) best case when list is almost ordered which should be the case most of the time.
	InsertionSort(transparentObjects.begin(), transparentObjects.end(), Scene::CompareByCameraDistanceInv);
	InsertionSort(opaqueObjects.begin(), opaqueObjects.end(), Scene::CompareByCameraDistance);
}

void Scene::UpdateFrustumCulling(Frustum& frustum, Vec3Graphics camPos){
	Vec3Graphics pos, dir;
	for (unsigned int i = 0; i < transparentObjects.size(); ++i)
	{
		
		if (!frustum.InsideFrustum(transparentObjects[i]))
		{
			transparentObjects[i]->m_RenderComponent->disabled = true;
		}
		transparentObjects[i]->m_RenderComponent->disabled = false;

		pos = transparentObjects[i]->GetWorldTransform().GetTranslation();
		dir = pos - camPos;
		transparentObjects[i]->m_CamDist = dir.Dot(dir);
	}
	for (unsigned int i = 0; i < opaqueObjects.size(); ++i)
	{
		if (!frustum.InsideFrustum(opaqueObjects[i]))
		{
			opaqueObjects[i]->m_RenderComponent->disabled = true;
		}
		opaqueObjects[i]->m_RenderComponent->disabled = false;
		
		pos = opaqueObjects[i]->GetWorldTransform().GetTranslation();
		dir = pos - camPos;
		opaqueObjects[i]->m_CamDist = dir.Dot(dir);
	}
}

void Scene::addLightObject(GameObject* obj)
{
	lightObjects.push_back(obj);
}