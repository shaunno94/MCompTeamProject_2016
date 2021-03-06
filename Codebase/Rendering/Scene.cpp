#include "Scene.h"
#include "Particle.h"
#include "Renderer.h"
#include "DebugDraw.h"
#include "ParticleManager.h"

Scene::Scene()
{
	cam = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));
}


Scene::~Scene()
{
	ClearObjects();

	delete cam;
	if (myControllers)
	{
		delete myControllers;
		myControllers = nullptr;
	}
}

GameObject* Scene::findGameObject(const std::string& objectName)
{
	for (auto obj : ghostObjects)
	{
		if (obj->m_Name == objectName)
		{
			return obj;
		}
	}
	for (auto obj : opaqueObjects)
	{
		if (obj->m_Name == objectName)
		{
			return obj;
		}
	}
	for (auto obj : transparentObjects)
	{
		if (obj->m_Name == objectName)
		{
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
	//else if (dynamic_cast<Particle*>(obj))
//		particleObjects.push_back(obj);
	else 
		ghostObjects.push_back(obj);
}

void Scene::UpdateScene(float dt)
{
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
	for (unsigned int i = 0; i < particleObjects.size(); ++i)
	{
		particleObjects[i]->OnUpdateObject(dt);
	}

	UpdateFrustumCulling(frustum, camPos);

	//Sort lists using insertion sort - O(N) best case when list is almost ordered which should be the case most of the time.
	InsertionSort(transparentObjects.begin(), transparentObjects.end(), Scene::CompareByCameraDistanceInv);
	InsertionSort(opaqueObjects.begin(), opaqueObjects.end(), Scene::CompareByCameraDistance);
}

void Scene::UpdateFrustumCulling(Frustum& frustum, Vec3Graphics camPos)
{
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

void Scene::Setup()
{

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(Renderer::GetInstance());
#endif
}

void Scene::Cleanup()
{
	PhysicsEngineInstance::Release();
#if DEBUG_DRAW
	DebugDraw::Release();
#endif
}

void Scene::ClearObjects()
{
	for (auto& obj : opaqueObjects)
	{
		if (obj)
		{
			delete obj;
			obj = nullptr;
		}
	}
	for (auto obj : transparentObjects)
	{
		if (obj)
		{
			delete obj;
			obj = nullptr;
		}
	}
	for (auto obj : ghostObjects)
	{
		if (obj)
		{
			delete obj;
			obj = nullptr;
		}
	}
	for (auto light : lightObjects)
	{
		if (light)
		{
			delete light;
			light = nullptr;
		}
	}
	transparentObjects.clear();
	opaqueObjects.clear();
	ghostObjects.clear();
	lightObjects.clear();
	
	if (myControllers)
		myControllers->clear();
}

void Scene::addParticleObject(GameObject* obj)
{
	particleObjects.push_back(obj);
}