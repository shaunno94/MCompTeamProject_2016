#include "Renderer.h"

#include "constants.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();
	camera = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));
	modelMatrix.SetTranslation(Vec3Graphics(0.0, 0.0, -2.0));
	//TODO: change SHADERDIR to SHADER_DIR
	currentShader = new Shader(SHADER_DIR"basicVertex.glsl", SHADER_DIR"colourFragment.glsl");

	if (!currentShader->IsOperational()) {
		return;
	}

	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	init = true;
}
Renderer::~Renderer(void)	{
	delete triangle;
}

void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//viewMatrix.ToIdentity();
	UpdateShaderMatrices();
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	triangle->Draw();
	glUseProgram(0);

	SwapBuffers();
}

void Renderer::RenderScene(Scene currentScene){
	//STUB
}