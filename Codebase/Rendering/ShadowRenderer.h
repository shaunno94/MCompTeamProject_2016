#pragma once
#include "OGLRenderer.h"
#include "Camera.h"
class ShadowRenderer :
	public OGLRenderer
{
public:
	ShadowRenderer(Window& parent);
	virtual ~ShadowRenderer(void);

	void RenderScene();
	void UpdateScene(float msec);

protected:
	void DrawShadowCasters();
	void DrawShapes();

	void SetupFBO();

	Mesh*	triangle;
	Mesh* floor;

	//Light info
	Mesh* lightEmitter;
	Vec3Graphics l_Position;
	Vec4Graphics l_colour;
	float l_radius;


	Camera*	camera;

	GLuint shadowTex;
	GLuint shadowFBO;
	Shader* sceneShader;
	Shader* shadowShader;


};

