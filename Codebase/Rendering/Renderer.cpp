#include "Renderer.h"

Renderer* Renderer::s_renderer = nullptr;

Renderer::Renderer(std::string title, int sizeX, int sizeY, bool fullScreen) : OGLRenderer(title, sizeX, sizeY, fullScreen)
{
	m_UpdateGlobalUniforms = true;

	currentShader = nullptr;

	aspectRatio = float(width) / float(height);
	localProjMat = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);

	currentScene = nullptr;
	init = true;
	if (!s_renderer)
		s_renderer = this;
	child = this;
}

Renderer::~Renderer(void)
{
	delete quad;
}

void Renderer::updateGlobalUniforms(Material* material)
{
	auto lightMat = dynamic_cast<LightMaterial*>(material);
	if (lightMat)
	{
		Vec3Graphics camPos = currentScene->getCamera()->GetPosition();
		auto test = lightMat->Set("cameraPos", camPos);
		lightMat->Set("pixelSize", Vec2Graphics(1.0f / width, 1.0f / height));
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
	}
	SwapBuffers();
}

void Renderer::OnUpdateScene(float dt, Frustum& frustum, Vec3Graphics camPos)
{
	currentScene->UpdateNodeLists(dt, frustum, camPos);
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
