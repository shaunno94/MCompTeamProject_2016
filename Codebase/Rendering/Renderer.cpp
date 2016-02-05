#include "Renderer.h"

Renderer* Renderer::s_renderer;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();
	camera = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));
	modelMatrix.SetTranslation(Vec3Graphics(0.0, 0.0, -2.0));
	//TODO: change SHADERDIR to SHADER_DIR
	currentShader = new Shader(SHADER_DIR"basicVertex.glsl", SHADER_DIR"colourFragment.glsl");

	if (!currentShader->IsOperational())
	{
		return;
	}

	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	currentScene = nullptr;
	init = true;
	if (!s_renderer)
		s_renderer = this;
}
Renderer::~Renderer(void)
{
	delete triangle;
}

void Renderer::UpdateScene(float msec)
{
	if (currentScene)
	{
		currentScene->getCamera()->UpdateCamera(msec);
		viewMatrix = currentScene->getCamera()->BuildViewMatrix();
	}
	else
	{
		camera->UpdateCamera(msec);
		viewMatrix = camera->BuildViewMatrix();
		//viewMatrix.ToIdentity();
	}
	UpdateShaderMatrices();
}

//TODO:: Probably needs to be seperate from UpdateScene call if you want to update the scene once and draw several times (like for the cube shadow maps)
void Renderer::RenderScene(float msec)
{
	UpdateScene(msec);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	//Draws all objects attatched to the current scene.
	if (currentScene)
	{
		//Draw opaques
		for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		{
			currentScene->getOpaqueObject(i)->OnUpdateObject(msec);
			currentScene->getOpaqueObject(i)->OnRenderObject();
			modelMatrix = currentScene->getOpaqueObject(i)->GetLocalTransform();
			UpdateModelMatrix();
		}
		//Draw transparent
		for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		{
			currentScene->getTransparentObject(i)->OnUpdateObject(msec);
			currentScene->getTransparentObject(i)->OnRenderObject();
			modelMatrix = currentScene->getTransparentObject(i)->GetLocalTransform();
			UpdateModelMatrix();
		}
	}
	else
	{
		triangle->Draw();
	}
	glUseProgram(0);

	SwapBuffers();
}