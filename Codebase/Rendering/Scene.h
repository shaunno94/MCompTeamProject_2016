#pragma once
#include "GameObject.h"
#include <vector>
#include "Camera.h"
class Scene
{
public:
	Scene();
	~Scene();

	//return types may be changed
	GameObject* getOpaqueObject(unsigned int i) { return opaqueObjects[i]; }
	
	GameObject* getTransparentObject(unsigned int i){ return transparentObjects[i]; }

	GameObject* getLightObject(unsigned int i){ return lightObjects[i]; }
	
	unsigned int getNumTransparentObjects() { return transparentObjects.size(); }
	unsigned int getNumOpaqueObjects() { return opaqueObjects.size(); }
	unsigned int getNumLightObjects() { return lightObjects.size(); }

	Camera* getCamera(){ return cam; }

	void addGameObject(GameObject* obj, bool transparent = false);

	unsigned char renderFlags; //triggers for  graphical effects such as post processing and any other info required by the renderer

private:
	std::vector<GameObject*> transparentObjects;
	std::vector<GameObject*> opaqueObjects;
	std::vector<GameObject*> lightObjects;
	Camera* cam;	
};

