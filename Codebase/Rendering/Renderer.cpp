#include "Renderer.h"

Renderer* Renderer::s_renderer;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();
	camera = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));

	currentShader = nullptr;
	//TODO: change SHADERDIR to SHADER_DIR
	/*currentShader = new Shader(SHADER_DIR"basicVertex.glsl", SHADER_DIR"colourFragment.glsl");

	if (!currentShader->IsOperational())
	{
		return;
	}*/

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
		//Updates all objects in the scene, sorts lists for rendering
		currentScene->UpdateNodeLists(msec);
	}
	else
	{
		camera->UpdateCamera(msec);
		viewMatrix = camera->BuildViewMatrix();
		//viewMatrix.ToIdentity();
	}
	//TODO: This will not work. As each object could have its own shader, the update needs to be called for each game object.
	//TODO: When we add UBOs this approach will be valid again, so no need to spend a lot of time fixing this issue.
	UpdateShaderMatrices();
}

//TODO:: Might need to be seperate from UpdateScene call if you want to update the scene once and draw several times (like for the cube shadow maps)
void Renderer::RenderScene(float msec)
{
	UpdateScene(msec);
	glClear(GL_COLOR_BUFFER_BIT);

	//glUseProgram(currentShader->GetProgram());

	//Draws all objects attatched to the current scene.
	if (currentScene)
	{
		//Draw opaques
		for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		{
			currentScene->getOpaqueObject(i)->OnRenderObject();
		}
		//Draw transparent
		for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		{
			currentScene->getTransparentObject(i)->OnRenderObject();
		}
	}
	else
	{
		triangle->Draw();
	}
	glUseProgram(0);

	SwapBuffers();
}