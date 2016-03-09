#include "Renderer.h"
#include "GUISystem.h"
#include "Helpers/MeasuringTimer.h"

Renderer* Renderer::s_renderer = nullptr;

Renderer::Renderer(std::string title, int sizeX, int sizeY, bool fullScreen) :
#ifndef ORBIS
	OGLRenderer(title, sizeX, sizeY, fullScreen)
#else
	PS4Renderer()
#endif
{
	m_UpdateGlobalUniforms = true;
	currentShader = nullptr;

	aspectRatio = float(sizeX) / float(sizeY);
	pixelPitch = Vec2Graphics(1.0f / float(sizeX), 1.0f / float(sizeY));
	localProjMat = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);

	currentScene = nullptr;

	if (!s_renderer)
		s_renderer = this;
	child = this;
}

Renderer::~Renderer(void) {}

void Renderer::updateGlobalUniforms(Material* material)
{
	auto lightMat = dynamic_cast<LightMaterial*>(material);
	if (lightMat)
	{
		lightMat->Set("cameraPos", currentScene->getCamera()->GetPosition());
		lightMat->Set("pixelSize", pixelPitch);
	}
}

void Renderer::UpdateScene(float msec)
{
	if (currentScene)
	{
		currentScene->getCamera()->UpdateCamera(msec);
		currentScene->UpdateScene(msec);
		viewMatrix = currentScene->getCamera()->BuildViewMatrix();
		//Updates all objects in the scene, sorts lists for rendering
		frameFrustrum.FromMatrix(projMatrix * viewMatrix);
		currentScene->UpdateNodeLists(msec, frameFrustrum, currentScene->getCamera()->GetPosition());
	}
	if (m_UpdateGlobalUniforms)
	{
		for (unsigned int i = 0; i < currentScene->getNumLightObjects(); ++i)
		{
			auto rc = currentScene->getLightObject(i)->GetRenderComponent();
			updateGlobalUniforms(rc->m_Material);
		}
		m_UpdateGlobalUniforms = false;
	}
}

void Renderer::RenderScene(float msec)
{
	projMatrix = localProjMat;
	UpdateScene(msec);

	//Draws all objects attatched to the current scene.
	if (currentScene)
	{

		FillBuffers(); //First Pass

		DrawPointLights(); //Second Pass

		CombineBuffers(); //Final Pass

		RenderGUI();

	}

	SwapBuffers();

}

void Renderer::RenderGUI()
{
	viewMatrix.ToIdentity();
	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, 1, -1);
	GetCurrentScene()->guiSystem->Render();
}

void Renderer::OnUpdateScene(Frustum& frustum, Vec3Graphics camPos)
{
	currentScene->UpdateFrustumCulling(frustum, camPos);
}

void Renderer::OnRenderScene()
{
	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		currentScene->getTransparentObject(i)->OnRenderObject();
}

void Renderer::OnRenderLights()
{
	Vec3Graphics camPos = currentScene->getCamera()->GetPosition();
	for (unsigned int i = 0; i < currentScene->getNumLightObjects(); ++i)
	{
		GameObject* light = currentScene->getLightObject(i);

		DrawShadow(light);

		LightMaterial* lm = static_cast<LightMaterial*>(light->GetRenderComponent()->m_Material);

		lm->Set("lightPos", light->GetWorldTransform().GetTranslation());
		lm->Set("lightRadius", light->GetBoundingRadius());
		lm->Set("lightColour", Vec4Graphics(1, 0.7, 0.5, 1));
		lm->Set("cameraPos", currentScene->getCamera()->GetPosition());
		lm->Set("shadowBias", lm->shadowBias);

		float dist = (light->GetWorldTransform().GetTranslation() - currentScene->getCamera()->GetPosition()).Length();

		if (dist < light->GetBoundingRadius())  // camera is inside the light volume !
			SetCullFace(FRONT);
		else
			SetCullFace(BACK);

		light->OnRenderObject();
	}
}
