#include "Renderer.h"
#include "GUISystem.h"

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
	//TODO: change SHADERDIR to SHADER_DIR

	aspectRatio = float(sizeX) / float(sizeY);
	pixelPitch = Vec2Graphics(1.0f / float(sizeX), 1.0f / float(sizeY));
	localProjMat = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);

	currentScene = nullptr;

	if (!s_renderer)
		s_renderer = this;
	child = this;
}

Renderer::~Renderer(void) {}

void Renderer::SetCurrentScene(Scene* s)
{
	currentScene = s;

	if (child->GetCurrentScene()->getCubeMapDir())
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

void Renderer::updateGlobalUniforms(Material* material)
{
	auto lightMat = dynamic_cast<LightMaterial*>(material);
	if (lightMat)
	{
		Vec3Graphics camPos = currentScene->getCamera()->GetPosition();
		auto test = lightMat->Set("cameraPos", camPos);
		lightMat->Set("pixelSize", pixelPitch);
		int i = 0;
	}
}

void Renderer::UpdateScene(float msec)
{
	if (currentScene)
	{
		currentScene->getCamera()->UpdateCamera(msec);
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
		//m_UpdateGlobalUniforms = false;
	}
	//TODO: This will not work. As each object could have its own shader, the update needs to be called for each game object.
	//TODO: When we add UBOs this approach will be valid again, so no need to spend a lot of time fixing this issue.
	UpdateShaderMatrices();
}

//TODO:: Might need to be seperate from UpdateScene call if you want to update the scene once and draw several times (like for the cube shadow maps)
void Renderer::RenderScene(float msec)
{
	projMatrix = localProjMat;
	UpdateScene(msec);

	//glUseProgram(currentShader->GetProgram());
	//Draws all objects attatched to the current scene.
	if (currentScene)
	{
		//Draw
		FillBuffers(); //First Pass
		DrawPointLights(); //Second Pass
		CombineBuffers(); //Final Pass
		GUISystem::GetInstance().Render();
	}
	SwapBuffers();
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
	for (unsigned int i = 0; i < currentScene->getNumLightObjects(); ++i)
	{
		GameObject* light = currentScene->getLightObject(i);
		DrawShadow(light);
		LightMaterial* lm = (LightMaterial*)light->GetRenderComponent()->m_Material;
		lm->Set("lightPos", light->GetWorldTransform().GetTranslation());
		lm->Set("lightRadius", light->GetBoundingRadius());
		lm->Set("lightColour", Vec4Graphics(1, 0.7, 0.5, 1));
		lm->Set("cameraPos", currentScene->getCamera()->GetPosition());
		lm->Set("shadowBias", lm->shadowBias);

		UpdateShaderMatrices();

		float dist = (light->GetWorldTransform().GetTranslation() - currentScene->getCamera()->GetPosition()).Length();

		if (dist < light->GetBoundingRadius())  // camera is inside the light volume !
			SetCullFace(FRONT);
		else
			SetCullFace(BACK);

		light->OnRenderObject();
	}
}
