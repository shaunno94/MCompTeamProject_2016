#pragma once
#include "Scene.h"
#define DEBUG_DRAW 1
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

	void Renderer::SetCurrentScene(Scene* s)
	{
		currentScene = s;

		if (Renderer::GetInstance()->GetCurrentScene()->getCubeMapDir())
		{

			skyBoxTex = SOIL_load_OGL_cubemap(
				child->GetCurrentScene()->getCubeMapDir()[0],
				child->GetCurrentScene()->getCubeMapDir()[1],
				child->GetCurrentScene()->getCubeMapDir()[2],
				child->GetCurrentScene()->getCubeMapDir()[3],
				child->GetCurrentScene()->getCubeMapDir()[4],
				child->GetCurrentScene()->getCubeMapDir()[5],
				SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
		}
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

	Frustum frameFrustrum;
	Frustum lightFrustrum;
	static Renderer* s_renderer;
	void updateGlobalUniforms(Material* material);

	bool m_UpdateGlobalUniforms;
	float aspectRatio;
	Mat4Graphics localProjMat;
	float windowHeight, windowWidth;
	Vec2Graphics pixelPitch;
	Scene* currentScene;
};
