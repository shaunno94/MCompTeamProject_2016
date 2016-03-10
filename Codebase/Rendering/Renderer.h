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
	static Renderer* GetInstance()
	{
		return s_renderer;
	}

	void SetCurrentScene(Scene* s)
	{
		currentScene = s;
	}
	Scene* GetCurrentScene()
	{
		return currentScene;
	}

	Renderer(std::string title, int sizeX, int sizeY, bool fullScreen);
	virtual ~Renderer(void);

	void RenderScene(float msec);
	void UpdateScene(float msec);

protected:
	void OnUpdateScene(Frustum& frustum, Vec3Graphics camPos);
	void OnRenderScene();
	void OnRenderLights();
	void RenderGUI();

	//virtual void Resize(int x, int y)override;

	Frustum frameFrustrum;
	Frustum lightFrustrum;
	static Renderer* s_renderer;
	
	float aspectRatio;
	Mat4Graphics localProjMat;
	float windowHeight, windowWidth;
	Vec2Graphics pixelPitch;
	Scene* currentScene;
};
