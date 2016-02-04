#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "Scene.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	void RenderScene();
	void RenderScene(Scene currentScene);
	void UpdateScene(float msec);

protected:

	//--Contained in Scene--//
	Mesh*	triangle;
	Camera*	camera;
	//----------------------//
};
