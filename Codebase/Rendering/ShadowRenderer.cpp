#include "ShadowRenderer.h"
#include "TextureSetup.h"

#include "constants.h"

ShadowRenderer::ShadowRenderer(Window& parent) : OGLRenderer(parent)
{
	TextureGroup texGroup;
	TextureSetup* texSet = texGroup.GetTexture("../../Textures/brick.tga");

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "texture"), texSet->Load());


	triangle = Mesh::GenerateTriangle();
	floor = Mesh::GenerateQuad();
	lightEmitter = Mesh::GenerateIcosphere(8);
	l_colour = Vec4Graphics(0, 1, 1, 1);
	l_Position = Vec3Graphics(0, 2, 0);
	l_radius = 100;

	camera = new Camera(0.0f, 0.0f, Vec3Graphics(0, 0, 0));
	modelMatrix.SetTranslation(Vec3Graphics(0.0, 0.0, -2.0));
	//TODO: change SHADERDIR to SHADER_DIR
	sceneShader = new Shader("../../Shaders/""shadowscenevertex.glsl", "../../Shaders/""shadowscenefragment - Copy.glsl");
	shadowShader = new Shader("../../Shaders/""shadowVertex.glsl", "../../Shaders/""shadowFragment.glsl");

	if (!sceneShader->IsOperational() || !shadowShader->IsOperational())
	{
		return;
	}

	SetupFBO();

	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	init = true;
}

ShadowRenderer::~ShadowRenderer(void)
{
	delete triangle;
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;
	delete lightEmitter;
	delete floor;
	delete sceneShader;
	delete shadowShader;
	currentShader = NULL;
}

void ShadowRenderer::SetupFBO()
{
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTex);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (size_t i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glEnable(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowRenderer::UpdateScene(float msec)
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		l_Position = l_Position + Vec3Graphics(1, 0, 0);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		l_Position = l_Position + Vec3Graphics(-1, 0, 0);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
		l_Position = l_Position + Vec3Graphics(0, 0, 1);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
		l_Position = l_Position + Vec3Graphics(0, 0, -1);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
		l_Position = l_Position + Vec3Graphics(0, 1, 0);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
		l_Position = l_Position + Vec3Graphics(0, -1, 0);
	}
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//viewMatrix.ToIdentity();
	UpdateShaderMatrices();
}

void ShadowRenderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	DrawShadowCasters();
	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	DrawShapes();

	SwapBuffers();
}

void ShadowRenderer::DrawShadowCasters() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glViewport(0, 0, 512, 512);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	Vec3Graphics directions[6];
	directions[0] = Vec3Graphics(1, 0, 0);
	directions[1] = Vec3Graphics(-1, 0, 0);
	directions[2] = Vec3Graphics(0, -1, 0);
	directions[3] = Vec3Graphics(0, 1, 0);
	directions[4] = Vec3Graphics(0, 0, -1);
	directions[5] = Vec3Graphics(0, 0, 1);

	Vec3Graphics up[6];
	up[0] = Vec3Graphics(0, 1, 0);
	up[1] = Vec3Graphics(0, 1, 0);
	up[2] = Vec3Graphics(0, 0, -1);
	up[3] = Vec3Graphics(0, 0, -1);
	up[4] = Vec3Graphics(0, 1, 0);
	up[5] = Vec3Graphics(0, 1, 0);


	projMatrix = Mat4Graphics::Perspective(1, 1000, 1, 90);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < 6; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, shadowTex, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		SetCurrentShader(shadowShader);
		//SetCurrentShader(sceneShader);
		viewMatrix = Mat4Graphics::View(
			l_Position, l_Position + directions[i], up[i]); //modify;

		/*if (i == 2)
		{
		viewMatrix = Matrix4::Rotation(-90.0f, Vector3(1.0f, 0.f, 0.f));
		viewMatrix.SetPositionVector(light->GetPosition());
		}*/


		textureMatrix = biasMatrix *(projMatrix * viewMatrix);

		UpdateShaderMatrices();

		//draw floor
		modelMatrix = (Mat4Graphics::Translation(Vec3Graphics(0, -1, 0)) *
			Mat4Graphics::Rotation(90, Vec3Graphics(1, 0, 0)) * 
			Mat4Graphics::Scale(Vec3Graphics(50, 50, 1)));
		Mat4Graphics tempMatrix = textureMatrix * modelMatrix;
	
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);
		floor->Draw();

		//draw triangle
		modelMatrix.ToIdentity();
		tempMatrix = textureMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);
		triangle->Draw();
		
	}
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowRenderer::DrawShapes() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 12);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos"), 1, (float*)&l_Position);
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour"), 1, (float*)&l_colour);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "lightRadius"), l_radius);


	Mat4Graphics lightProj = Mat4Graphics::Perspective(1, 1000, 1, 90);
	Mat4Graphics lightView = Mat4Graphics::Translation(-l_Position);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "lightProj"), 1, false, (float*)&lightProj);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "lightView"), 1, false, (float*)&lightView);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	//draw floor
	modelMatrix = (Mat4Graphics::Translation(Vec3Graphics(0, -1, 0)) *
		Mat4Graphics::Rotation(90, Vec3Graphics(1, 0, 0)) *
		Mat4Graphics::Scale(Vec3Graphics(50, 50, 1)));
	Mat4Graphics tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);
	floor->Draw();

	//draw triangle
	modelMatrix.ToIdentity();
	tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);
	triangle->Draw();

	//draw lightEmitter
	modelMatrix = (Mat4Graphics::Translation(l_Position) * Mat4Graphics::Scale(Vec3Graphics(4, 4, 4)));
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);
	lightEmitter->Draw();

	glUseProgram(0);
}