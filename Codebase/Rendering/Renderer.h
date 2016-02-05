#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "Scene.h"
#include "constants.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	void RenderScene(float msec);
	void SetCurrentScene(Scene* s) { currentScene = s; }

protected:
	void UpdateScene(float msec);

	Scene* currentScene;
	//--Contained in Scene--//
	Mesh*	triangle;
	Camera*	camera;
	//----------------------//
};
