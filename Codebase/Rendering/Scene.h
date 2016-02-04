#pragma once
#include "GameObject.h"//TODO - correct to real file
#include <vector>
#include "Camera.h"
class Scene
{
public:
	Scene();
	~Scene();

	//return types may be changed
	//GameObject* getOpaqueObjects(); 
	//GameObject* getTransparentObjects();
	//GameObject* getAllObjects();

	Camera* getCamera(){ return cam; }

	int renderFlags; //triggers for  graphical effects such as post processing and any other info required by the renderer

private:
	//std::vector<GameObject> objects;
	Camera* cam;	
};

