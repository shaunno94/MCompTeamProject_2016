#pragma once
#include "Scene.h"
#define DEBUG_DRAW 0
/// @ingroup Rendering
/// <summary>
/// Non-platform specific functionality for rendering a <see cref="Scene"/>.
/// </summary>
#ifndef ORBIS
#include "OGLRenderer.h"
class Renderer : public OGLRenderer
{
	friend class OGLRenderer;
#else
#include "PS4Renderer.h"
class Renderer : public PS4Renderer 
{
	friend class PS4Renderer;
#endif
public:
	static Renderer* GetInstance() { return s_renderer; }	
	void SetCurrentScene(Scene* s) { currentScene = s; }

	Renderer(std::string title, int sizeX, int sizeY, bool fullScreen);
	virtual ~Renderer(void);

	void RenderScene(float msec);
	void UpdateScene(float msec);

protected:
	void OnUpdateScene();
	void OnRenderScene();
	void OnRenderLights();

	Frustum frameFrustrum;
	static Renderer* s_renderer;
	void updateGlobalUniforms(Material* material);

	bool m_UpdateGlobalUniforms;
	float aspectRatio;
	Mat4Graphics localProjMat;
	float windowHeight, windowWidth;
	Scene* currentScene;
};
