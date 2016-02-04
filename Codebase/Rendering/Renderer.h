#pragma once

#include "OGLRenderer.h"
#include "Camera.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	void RenderScene();
	void UpdateScene(float msec);

protected:
	Mesh*	triangle;
	Camera*	camera;
};
