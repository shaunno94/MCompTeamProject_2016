#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "Scene.h"
#include "constants.h"

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
	//--Contained in Scene--//
	Mesh*	triangle;
	Camera*	camera;

	static Renderer* s_renderer;
	//----------------------//

	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void initFBO();
	void GenerateScreenTexture(GLuint & into, bool depth = true);


	GLuint bufferFBO; // FBO for G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; // FBO for lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	RenderComponent* quad;
};
