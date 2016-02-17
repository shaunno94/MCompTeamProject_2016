#pragma once

#include "Camera.h"
#include "Scene.h"
#include "constants.h"
#define DEBUG_DRAW 1

/// @ingroup Rendering
/// <summary>
/// Non-platform specific functionality for rendering a <see cref="Scene"/>.
/// </summary>
#ifndef ORBIS
#include "OGLRenderer.h"
class Renderer : public OGLRenderer
#else
#include "PS4Renderer.h"
class Renderer : public PS4Renderer 
#endif
{
public:

	static Renderer* GetInstance()
	{
		return s_renderer;
	}

	Renderer(std::string title, int sizeX, int sizeY, bool fullScreen);
	virtual ~Renderer(void);

	void RenderScene(float msec);
	void SetCurrentScene(Scene* s)
	{
		currentScene = s;
	}
	void UpdateScene(float msec);

protected:
	Scene* currentScene;
	Frustum frameFrustrum;
	//--Contained in Scene--//
	Camera*	camera;

	static Renderer* s_renderer;
	//----------------------//

	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void initFBO();
	void GenerateScreenTexture(GLuint & into, bool depth = false);

	void updateGlobalUniforms(Material* material);

	GLuint bufferFBO; // FBO for G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; // FBO for lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	GameObject* quad;

	bool m_UpdateGlobalUniforms;
	float aspectRatio;
	Mat4Graphics localProjMat;
	float windowHeight, windowWidth;
};
