#pragma once
#include "GameObject.h"
#include <vector>
#include "Camera.h"
#include "Helpers\collections.h"
#include "Frustum.h"

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
	~Scene();

	GameObject* findGameObject(const std::string& objectName);
	GameObject* getOpaqueObject(unsigned int i) { return opaqueObjects[i]; }

	GameObject* getTransparentObject(unsigned int i){ return transparentObjects[i]; }

	GameObject* getLightObject(unsigned int i){ return lightObjects[i]; }

	unsigned int getNumTransparentObjects() { return transparentObjects.size(); }
	unsigned int getNumOpaqueObjects() { return opaqueObjects.size(); }
	unsigned int getNumLightObjects() { return lightObjects.size(); }

	Camera* getCamera(){ return cam; }

	//attaches the camera to a game object
	void attachCam(GameObject* player){
		if (cam)
			delete cam;
		cam = new Camera(player);
	}
	void addGameObject(GameObject* obj);
	void addLightObject(GameObject* obj);

	unsigned char renderFlags; //triggers for graphical effects such as post processing and any other info required by the renderer

private:
	//Comparators for sorting node lists - opaques (front to back), transparents (back to front). 
	//Uses distance from camera for comparison.
	static bool CompareByCameraDistance(const GameObject* a, const GameObject* b) { return (a->m_CamDist < b->m_CamDist); }
	static bool CompareByCameraDistanceInv(const GameObject* a, const GameObject* b) { return (a->m_CamDist > b->m_CamDist); }
	//Updates game objects by calling OnUpdateObject and then sorts the render lists for opaque and transparent objects.
	void UpdateNodeLists(float dt, Frustum& frustum, Vec3Graphics camPos);
	void UpdateFrustumCulling(Frustum& frustum, Vec3Graphics camPos);

	std::vector<GameObject*> transparentObjects;
	std::vector<GameObject*> opaqueObjects;
	std::vector<GameObject*> lightObjects;
	Camera* cam;
};

