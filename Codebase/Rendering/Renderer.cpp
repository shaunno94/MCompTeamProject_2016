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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initFBO();

	currentScene = nullptr;
	init = true;
	if (!s_renderer)
		s_renderer = this;
}

Renderer::~Renderer(void)
{
	delete triangle;
	delete quad;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}

void Renderer::initFBO()
{
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate Screen sized textures ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	//Second Pass
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	quad = new RenderComponent(new Material(new Shader(SHADER_DIR"combinevert.glsl", SHADER_DIR"combinefrag.glsl")), Mesh::GenerateQuad());
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
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
	//TODO: This will not work. As each object could have its own shader, the update needs to be called for each game object.
	//TODO: When we add UBOs this approach will be valid again, so no need to spend a lot of time fixing this issue.
	UpdateShaderMatrices();
}

//TODO:: Might need to be seperate from UpdateScene call if you want to update the scene once and draw several times (like for the cube shadow maps)
void Renderer::RenderScene(float msec)
{
	UpdateScene(msec);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//glUseProgram(currentShader->GetProgram());

	//Draws all objects attatched to the current scene.
	if (currentScene)
	{
		//Update
		for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
			currentScene->getOpaqueObject(i)->OnUpdateObject(msec);
		for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
			currentScene->getTransparentObject(i)->OnUpdateObject(msec);


		//do stuff

		FillBuffers(); //First Pass
		DrawPointLights(); //Second Pass
		CombineBuffers(); //Final Pass

		//Draw
		for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
			currentScene->getOpaqueObject(i)->OnRenderObject();
		for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
			currentScene->getTransparentObject(i)->OnRenderObject();

		this->currentShader = // more suff
			nullptr;

		//combination
	}
	else
	{
		//triangle->Draw();
	}
	glUseProgram(0);

	SwapBuffers();
}

void Renderer::FillBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//SetCurrentShader(sceneShader);


	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		currentScene->getTransparentObject(i)->OnRenderObject();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights()
{
	//SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);
	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.DEPTH.index);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for (unsigned int i = 0; i < currentScene->getNumLightObjects(); ++i)
	{
		GameObject* light = currentScene->getLightObject(i);
		Renderer::UpdateUniform(glGetUniformLocation(light->GetRenderComponent()->m_Material->GetShader()->GetProgram(), "lightPos"), light->GetWorldTransform().GetTranslation());
		Renderer::UpdateUniform(glGetUniformLocation(light->GetRenderComponent()->m_Material->GetShader()->GetProgram(), "lightRagius"), light->GetBoundingRadius());
		Renderer::UpdateUniform(glGetUniformLocation(light->GetRenderComponent()->m_Material->GetShader()->GetProgram(), "lightColour"), Vec4Graphics(0,0,0,0));

		

		UpdateShaderMatrices();

		float dist = (light->GetWorldTransform().GetTranslation() - camera->GetPosition()).Length();
		if (dist < light->GetBoundingRadius())  // camera is inside the light volume !
		{
			glCullFace(GL_FRONT);
		}
		else
		{
			glCullFace(GL_BACK);
		}
		currentScene->getLightObject(i)->OnRenderObject();
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::CombineBuffers() {
	//SetCurrentShader(combineShader);

	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 8);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 9);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 10);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	quad->Draw();
	glUseProgram(0);
}