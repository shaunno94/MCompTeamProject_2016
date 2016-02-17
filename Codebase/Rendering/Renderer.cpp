#include "Renderer.h"
#include "LightMaterial.h"

Renderer* Renderer::s_renderer;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	m_UpdateGlobalUniforms = true;

	currentShader = nullptr;

	aspectRatio = float(width) / float(height);
	localProjMat = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);
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
	delete quad;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &ShadowTex2D);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::initFBO()
{
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &shadowFBO);

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
		throw "!GL_FRAMEBUFFER_COMPLETE";
	}

	//Second Pass
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw "!GL_FRAMEBUFFER_COMPLETE";
	}

	//Generate Shadow Texture
	glGenTextures(1, &ShadowTex2D);
	glBindTexture(GL_TEXTURE_2D, ShadowTex2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,	GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	//attach to shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,	GL_TEXTURE_2D, ShadowTex2D, 0);
	glDrawBuffer(GL_NONE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	quad = new GameObject();
	quad->SetRenderComponent(new RenderComponent(new LightMaterial(new Shader(SHADER_DIR"combinevert.glsl", SHADER_DIR"combinefrag.glsl")), Mesh::GenerateQuad()));
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.EMISSIVE.name, (int)ReservedOtherTextures.EMISSIVE.index);
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.SPECULAR.name, (int)ReservedOtherTextures.SPECULAR.index);
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//glUseProgram(currentShader->GetProgram());
	//Draws all objects attatched to the current scene.
	if (currentScene)
	{
		//Draw
		FillBuffers(); //First Pass
		DrawPointLights(); //Second Pass
		CombineBuffers(); //Final Pass
	}
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::FillBuffers()
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//SetCurrentShader(sceneShader);

	UpdateShaderMatrices();

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->debugDrawWorld();
#endif

	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
			currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
			currentScene->getTransparentObject(i)->OnRenderObject();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
}

void Renderer::DrawPointLights()
{
	//SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.DEPTH.index);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.NORMALS.index);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

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
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);

		light->OnRenderObject();
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

	glActiveTexture(GL_TEXTURE0 + ReservedMeshTextures.DIFFUSE.index);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.EMISSIVE.index);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SPECULAR.index);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	quad->m_RenderComponent->Draw();

	glUseProgram(0);
}

void Renderer::DrawShadow(GameObject* light){
	LightMaterial* lm = (LightMaterial*)(light->GetRenderComponent()->m_Material);
	switch (lm->shadowType)
	{
	case _NONE:
	default:
		break;
	case _2D:
		DrawShadow2D(light);
		break;
	//case _CUBE:
	}
}
void Renderer::DrawShadow2D(GameObject* light){
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	projMatrix = Mat4Graphics::Perspective(50.0f, 15000.0f, 1.0f, 45.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//TODO override shader
	//SetCurrentShader ( shadowShader );
	viewMatrix = Mat4Graphics::View(light->GetWorldTransform().GetTranslation(), Vec3Graphics(0, 0, 0));
	Mat4Graphics model = Mat4Graphics::Scale(Vec3Graphics(80, 40, 40)) * Mat4Graphics::Rotation(90.0f, Vec3Graphics(1, 0, 0));
	((LightMaterial*)light->GetRenderComponent()->m_Material)->shadowBias = biasMatrix *(projMatrix*viewMatrix);

	//draw game objects
	lightFrustrum.FromMatrix(projMatrix * viewMatrix);
	currentScene->UpdateNodeLists(0, lightFrustrum, light->GetWorldTransform().GetTranslation());

	glDisable(GL_CULL_FACE);
	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		currentScene->getTransparentObject(i)->OnRenderObject();
	glEnable(GL_CULL_FACE);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	viewMatrix = currentScene->getCamera()->BuildViewMatrix();

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SHADOW_2D.index);
	glBindTexture(GL_TEXTURE_2D, ShadowTex2D); 
	glActiveTexture(GL_TEXTURE0);

	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);
	viewMatrix = currentScene->getCamera()->BuildViewMatrix();
	frameFrustrum.FromMatrix(projMatrix * viewMatrix);
	currentScene->UpdateNodeLists(0, frameFrustrum, currentScene->getCamera()->GetPosition());
}