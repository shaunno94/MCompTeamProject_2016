#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "Scene.h"
#include "constants.h"
#define DEBUG_DRAW 1

class Renderer : public OGLRenderer	{
public:

	static Renderer* GetInstance() { return s_renderer; }

	Renderer(Window &parent);
	virtual ~Renderer(void);

	void RenderScene(float msec);
	void SetCurrentScene(Scene* s) { currentScene = s; }
	void UpdateScene(float msec);

protected:

	Scene* currentScene;
	Frustum frameFrustrum;
	//--Contained in Scene--//
	Mesh*	triangle;
	Camera*	camera;

	static Renderer* s_renderer;
	//----------------------//
};
