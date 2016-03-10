#pragma once
#include "GameObject.h"
#include <vector>
#include "Camera.h"
#include "Helpers\collections.h"
#include "Frustum.h"
#include "GUISystem.h"

class Renderer;

/// @ingroup Rendering
/// <summary>
///
/// </summary>
class Scene
{
	friend class Renderer;
public:
	Scene();
	virtual ~Scene();

	GameObject* findGameObject(const std::string& objectName);
	GameObject* getOpaqueObject(unsigned int i)
	{
		return opaqueObjects[i];
	}

	GameObject* getTransparentObject(unsigned int i)
	{
		return transparentObjects[i];
	}

	GameObject* getLightObject(unsigned int i)
	{
		return lightObjects[i];
	}
	GameObject* getGhostObject(unsigned int i)
	{
		return ghostObjects[i];
	}

	unsigned int getNumTransparentObjects()
	{
		return transparentObjects.size();
	}
	unsigned int getNumOpaqueObjects()
	{
		return opaqueObjects.size();
	}
	unsigned int getNumLightObjects()
	{
		return lightObjects.size();
	}
	const char** getCubeMapDir()
	{
		return cubemapDir;
	}
	unsigned int getNumGhostObjects()
	{
		return ghostObjects.size();
	}

	Camera* getCamera()
	{
		return cam;
	}

	//attaches the camera to a game object
	void attachCam(GameObject* player)
	{
		if (cam)
			delete cam;
		cam = new Camera(player);
	}
	void addGameObject(GameObject* obj);
	virtual void UpdateScene(float dt);
	void addLightObject(GameObject* obj);
	void setCubeMap(const char** dir)
	{
		cubemapDir = dir;
	}
	virtual GUISystem* getGUISystem()=0;

	unsigned char renderFlags; //triggers for graphical effects such as post processing and any other info required by the renderer

protected:
	//Comparators for sorting node lists - opaques (front to back), transparents (back to front).
	//Uses distance from camera for comparison.
	static bool CompareByCameraDistance(const GameObject* a, const GameObject* b)
	{
		return (a->m_CamDist < b->m_CamDist);
	}
	static bool CompareByCameraDistanceInv(const GameObject* a, const GameObject* b)
	{
		return (a->m_CamDist > b->m_CamDist);
	}
	//Updates game objects by calling OnUpdateObject and then sorts the render lists for opaque and transparent objects.
	void UpdateNodeLists(float dt, Frustum& frustum, Vec3Graphics camPos);
	void UpdateFrustumCulling(Frustum& frustum, Vec3Graphics camPos);

	std::vector<GameObject*> transparentObjects;
	std::vector<GameObject*> opaqueObjects;
	std::vector<GameObject*> lightObjects;
	std::vector<GameObject*> ghostObjects;
	Camera* cam;
	const char** cubemapDir;
};

