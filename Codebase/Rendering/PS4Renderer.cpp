#ifdef ORBIS
#include "PS4Renderer.h"
#include <video_out.h>	//Video System
#include <gnmx\basegfxcontext.h>
#include "Renderer.h"
#include "DebugDraw.h"

Renderer* PS4Renderer::child = nullptr;

PS4Renderer::PS4Renderer()
{
	currentGPUBuffer = 0;
	currentScreenBuffer = BACK_BUFFER1;
	prevScreenBuffer = 0;
	projMatrix.ToIdentity();
	viewMatrix.ToIdentity();

	currentShader = nullptr;
	currentGFXContext = nullptr;

	InitialiseMemoryAllocators();
	InitialiseGCMRendering();
	InitialiseVideoSystem();

	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModeAnisoBilinear);
	trilinearSampler.setAnisotropyRatio(sce::Gnm::kAnisotropyRatio16);
	trilinearSampler.setWrapMode(sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap);

	shadowSampler.init();
	shadowSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	shadowSampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);
	shadowSampler.setDepthCompareFunction(sce::Gnm::kDepthCompareLessEqual);
	shadowSampler.setWrapMode(sce::Gnm::kWrapModeClampLastTexel, sce::Gnm::kWrapModeClampLastTexel, sce::Gnm::kWrapModeClampLastTexel);

	primitiveSetup.init();
	primitiveSetup.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(sce::Gnm::kPrimitiveSetupFrontFaceCcw);

	dsc.init();	
	dsc.setDepthEnable(true);
	dsc.setDepthControl(sce::Gnm::kDepthControlZWriteEnable, sce::Gnm::kCompareFuncLessEqual);
	
	blendControl.init();
	blendControl.setBlendEnable(true);
	blendControl.setColorEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOneMinusSrcAlpha);

	dEqaaControl.init();
	dEqaaControl.setAlphaToMaskSamples(sce::Gnm::NumSamples::kNumSamples4);

	cc.init();

	dbRenderControl.init();
	dbRenderControl.setDepthTileWriteBackPolicy(sce::Gnm::kDbTileWriteBackPolicyCompressionAllowed);

	specularLoc.id = 2;
	specularLoc.stage = sce::Gnm::kShaderStagePs;

	emissiveLoc.id = 1;
	emissiveLoc.stage = sce::Gnm::kShaderStagePs;

	diffuseLoc.id = 0;
	diffuseLoc.stage = sce::Gnm::kShaderStagePs;

	depthLoc.id = 0;
	depthLoc.stage = sce::Gnm::kShaderStagePs;

	normalLoc.id = 1;
	normalLoc.stage = sce::Gnm::kShaderStagePs;

	shadowLoc.id = 2;
	shadowLoc.stage = sce::Gnm::kShaderStagePs;

	//Skybox setup
	skyQuad = new GameObject();
	skyQuad->SetRenderComponent(new RenderComponent(new Material(new PS4Shader(SHADER_DIR"skyboxVertex.sb", SHADER_DIR"skyboxFrag.sb")), Mesh::GenerateQuad()));
	skyQuad->GetRenderComponent()->m_Material->Set(ReservedOtherTextures.CUBE.name, Texture::Get(TEXTURE_DIR"skybox", true));

	//quad for final render
	fullScreenQuad = new GameObject();
	fullScreenQuad->SetRenderComponent(new RenderComponent(new LightMaterial(new PS4Shader(SHADER_DIR"combineVert.sb", SHADER_DIR"combineFrag.sb")), Mesh::GenerateQuad()));
	init = true;

	if (!fullScreenQuad->GetRenderComponent()->m_Material->GetShader()->IsOperational())
	{
		std::cerr << "Combine Shader is not operational..." << std::endl;
		init = false;
	}
	if (!skyQuad->GetRenderComponent()->m_Material->GetShader()->IsOperational())
	{
		std::cerr << "Skybox Shader is not operational..." << std::endl;
		init = false;
	}
	SwapBuffers();	
}

void PS4Renderer::InitialiseVideoSystem()
{
	//Initialise buffers - render targets.
	offScreenBuffers.resize(MAX_FBO);
	offScreenBuffers[G_BUFFER] = new PS4Buffer(width, height, stackAllocators[GARLIC], 2, false, NONE);
	offScreenBuffers[LIGHT_BUFFER] = new PS4Buffer(width, height, stackAllocators[GARLIC], 2, false, NONE);
	offScreenBuffers[SHADOW_BUFFER] = new PS4Buffer(SHADOWSIZE, SHADOWSIZE, stackAllocators[GARLIC], 0, false, NONE);

	screenBuffers.resize(MAX_BACK_BUFFER);
	screenBuffers[BACK_BUFFER1] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, false, FIRST_ONLY);
	screenBuffers[BACK_BUFFER2] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, false, FIRST_ONLY);
	screenBuffers[BACK_BUFFER3] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, false, FIRST_ONLY);	
	
	//open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);	

	for (uint i = 0; i < MAX_TARGETS_PER_BUFFER; ++i)
	{
		RegisterTargets(i, screenBuffers);
	}	
	for (uint i = 0; i < MAX_TARGETS_PER_BUFFER; ++i)
	{
		RegisterTargets(i, offScreenBuffers);
	}
}

void PS4Renderer::RegisterTargets(const uint targetIndex, std::vector<PS4Buffer*>& buffer)
{
	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute, SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB, SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9, (*buffer[0]->GetTarget(targetIndex)).getWidth(), (*buffer[0]->GetTarget(targetIndex)).getHeight(), (*buffer[0]->GetTarget(targetIndex)).getPitch());
	
	std::vector<void*> bufferAddresses;
	for (int i = 0; i < buffer.size(); ++i)
	{
		if (buffer[i]->GetTarget(targetIndex))
			bufferAddresses.push_back((*buffer[i]->GetTarget(targetIndex)).getBaseAddress());
	}
	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses.data(), bufferAddresses.size(), &attribute);
}

void PS4Renderer::InitialiseGCMRendering()
{
	frames = (PS4Frame*)onionAllocator.allocate(sizeof(PS4Frame)* _MaxCMDBufferCount, alignof(PS4Frame));

	for (int i = 0; i < _MaxCMDBufferCount; ++i)
	{
		new (&frames[i])PS4Frame();
	}

	sce::Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(onionAllocator, garlicAllocator, ownerHandle);
	sce::Gnmx::Toolkit::initializeWithAllocators(&allocators);
}

void PS4Renderer::InitialiseMemoryAllocators()
{
	stackAllocators[GARLIC].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);
	stackAllocators[ONION].init(SCE_KERNEL_WB_ONION, _OnionMemory);
	
	stackAllocators[GARLIC_MESH].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);

	this->garlicAllocator = sce::Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC]);
	this->onionAllocator = sce::Gnmx::Toolkit::GetInterface(&stackAllocators[ONION]);

	this->meshGarlicAllocator = sce::Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC_MESH]);

	sce::Gnm::registerOwner(&ownerHandle, "PS4Renderer");
}

void PS4Renderer::DestroyMemoryAllocators() 
{
	stackAllocators[GARLIC].deinit();
	stackAllocators[GARLIC_MESH].deinit();
	stackAllocators[ONION].deinit();
}

void	PS4Renderer::DestroyGCMRendering() 
{
	onionAllocator.release(frames);
}

void	PS4Renderer::DestroyVideoSystem() 
{
	for (int i = 0; i < MAX_BACK_BUFFER; ++i)
	{
		delete screenBuffers[i];
	}
	sceVideoOutClose(videoHandle);
}

PS4Renderer::~PS4Renderer()
{
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void PS4Renderer::SetTextureFlags(textureHandle&, unsigned int flags)
{
	// Repeat/Clamp options
	if ((flags & REPEATING) == REPEATING) 
	{
		trilinearSampler.setWrapMode(sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap);
	}
	else if ((flags & CLAMPING) == CLAMPING) 
	{
		trilinearSampler.setWrapMode(sce::Gnm::kWrapModeClampBorder, sce::Gnm::kWrapModeClampBorder, sce::Gnm::kWrapModeClampBorder);
	}
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
}

void PS4Renderer::SetCurrentShader(BaseShader* s)
{	
	if (s != currentShader)
		currentShader = static_cast<PS4Shader*>(s);

	currentShader->SubmitShaderSwitch(*currentGFXContext);
}

void PS4Renderer::InitCMD(PS4Buffer* buffer)
{
	currentGFXContext->setDbRenderControl(dbRenderControl);
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
	currentGFXContext->setDepthStencilControl(dsc);
	currentGFXContext->setupScreenViewport(0, 0, buffer->GetBufferWidth(), buffer->GetBufferHeight(), 0.5f, 0.5f);
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
	currentGFXContext->setBlendControl(0, blendControl);
	currentGFXContext->setDepthEqaaControl(dEqaaControl);
	currentGFXContext->setAaSampleCount(sce::Gnm::NumSamples::kNumSamples4, 100);
	currentGFXContext->setClipControl(cc);
}

void PS4Renderer::FillBuffers()
{
	offScreenBuffers[G_BUFFER]->ClearBuffer(*currentGFXContext);
	offScreenBuffers[G_BUFFER]->SetRenderTargets(*currentGFXContext);
	
	DrawSkyBox();
	
	InitCMD(offScreenBuffers[G_BUFFER]);

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->debugDrawWorld();
	DebugDraw::Instance()->RenderLine();
#endif

	child->OnRenderScene(false);
}

void PS4Renderer::DrawSkyBox()
{
	dsc.setDepthEnable(false);

	InitCMD(offScreenBuffers[G_BUFFER]);

	skyQuad->GetRenderComponent()->Draw();

	dsc.setDepthEnable(true);
}

void PS4Renderer::DrawPointLights()
{
	offScreenBuffers[LIGHT_BUFFER]->ClearBuffer(*currentGFXContext);
	offScreenBuffers[LIGHT_BUFFER]->SetRenderTargets(*currentGFXContext);

	blendControl.setAlphaEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOne);
	InitCMD(offScreenBuffers[LIGHT_BUFFER]);
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 1, 1, &shadowSampler);
	
	sce::Gnmx::decompressDepthSurface(currentGFXContext, offScreenBuffers[G_BUFFER]->GetDepthTarget());
	SetTexture(depthLoc, *offScreenBuffers[G_BUFFER]->GetTexture(DEPTH));

	SetTexture(normalLoc, *offScreenBuffers[G_BUFFER]->GetTexture(COLOUR + 1));

	sce::Gnmx::decompressDepthSurface(currentGFXContext, offScreenBuffers[SHADOW_BUFFER]->GetDepthTarget());
	SetTexture(shadowLoc, *offScreenBuffers[SHADOW_BUFFER]->GetTexture(DEPTH));
	
	child->OnRenderLights();
	
	primitiveSetup.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	blendControl.setColorEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOneMinusSrcAlpha);
}

void PS4Renderer::CombineBuffers()
{
	currentPS4Buffer->ClearBuffer(*currentGFXContext);
	currentPS4Buffer->SetRenderTargets(*currentGFXContext);

	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, 1, -1);
	InitCMD(currentPS4Buffer);

	SetTexture(diffuseLoc, *offScreenBuffers[G_BUFFER]->GetTexture(COLOUR));
	SetTexture(emissiveLoc, *offScreenBuffers[LIGHT_BUFFER]->GetTexture(COLOUR));
	SetTexture(specularLoc, *offScreenBuffers[LIGHT_BUFFER]->GetTexture(COLOUR + 1));

	fullScreenQuad->GetRenderComponent()->Draw();
}

void PS4Renderer::DrawShadow(GameObject* light)
{
	LightMaterial* lm = (LightMaterial*)(light->GetRenderComponent()->m_Material);
	switch (lm->shadowType)
	{
	case _NONE:
		break;
	case _2D:
		offScreenBuffers[SHADOW_BUFFER]->ClearBuffer(*currentGFXContext);
		offScreenBuffers[SHADOW_BUFFER]->SetRenderTargets(*currentGFXContext);
		InitCMD(offScreenBuffers[SHADOW_BUFFER]);
		
		DrawShadow2D(light);
		
		offScreenBuffers[LIGHT_BUFFER]->SetRenderTargets(*currentGFXContext);
		InitCMD(offScreenBuffers[LIGHT_BUFFER]);
		//currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 1, 1, &shadowSampler);
		break;
	case _CUBE:
		DrawShadowCube(light);
		break;
	}
}

void PS4Renderer::DrawShadow2D(GameObject* light)
{
	projMatrix = shadowProj;

	viewMatrix = Mat4Graphics::View(light->GetWorldTransform().GetTranslation(), Vec3Graphics(0, 0, 0));
	((LightMaterial*)light->GetRenderComponent()->m_Material)->shadowBias = biasMatrix * (projMatrix * viewMatrix); /*(Matrix4Simple::Translation(Vector3Simple(0.5f, 0.5f, 0.5f)) * Matrix4Simple::Scale(Vector3Simple(0.5f, -0.5f, 0.5f)))*/

	child->lightFrustrum.FromMatrix(projMatrix * viewMatrix);
	child->OnUpdateScene(child->lightFrustrum, light->GetWorldTransform().GetTranslation());

	child->OnRenderScene(true);

	viewMatrix = child->currentScene->getCamera()->BuildViewMatrix();
	projMatrix = child->localProjMat;

	child->OnUpdateScene(child->frameFrustrum, child->currentScene->getCamera()->GetPosition());
	//sce::Gnmx::decompressDepthSurface(currentGFXContext, offScreenBuffers[SHADOW_BUFFER]->GetDepthTarget());
}

void PS4Renderer::DrawShadowCube(GameObject* light)
{

}

//TODO:fix id here
void PS4Renderer::SetTexture(const shaderResourceLocation& location, textureHandle& handle)
{
	if (location.id >= 0)
	{
		currentGFXContext->setTextures(sce::Gnm::kShaderStagePs, location.id, 1, &handle);
	}
}

void PS4Renderer::SwapBuffers()
{
	if (currentFrame)
		currentFrame->EndFrame();

	SwapScreenBuffer();
	SwapCommandBuffer();

	currentFrame->StartFrame();
	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);
}

void PS4Renderer::SwapCommandBuffer()
{
	if (currentGFXContext) 
	{
		currentGFXContext->submit();
		sce::Gnm::submitDone();
	}

	currentGPUBuffer = (currentGPUBuffer + 1) % _MaxCMDBufferCount;

	currentFrame = &frames[currentGPUBuffer];
	currentGFXContext = &currentFrame->GetCommandBuffer();
}

void PS4Renderer::SwapScreenBuffer()
{
	prevScreenBuffer = currentScreenBuffer;

	currentScreenBuffer = (currentScreenBuffer + 1) % MAX_BACK_BUFFER;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);
	
	currentPS4Buffer = screenBuffers[currentScreenBuffer];
}

void PS4Renderer::SetCullFace(CULL c)
{
	primitiveSetup.setCullFace(c == FRONT ? sce::Gnm::kPrimitiveSetupCullFaceFront : sce::Gnm::kPrimitiveSetupCullFaceBack);
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
}

void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, const Mat4Graphics& mat4)
{
	if (location.id >= 0)
	{
		Mat4Graphics* matrix = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*matrix = mat4;
		sce::Gnm::Buffer constantBuffer; 
		constantBuffer.initAsConstantBuffer(matrix, sizeof(Mat4Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, const Mat3Graphics& mat3)
{
	if (location.id >= 0)
	{
		Mat3Graphics* matrix = (Mat3Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat3Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*matrix = mat3;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(matrix, sizeof(Mat3Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, const Vec4Graphics& vec4)
{
	if (location.id >= 0)
	{
		Vec4Graphics* vec = (Vec4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec4;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec4Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, const Vec3Graphics& vec3)
{
	if (location.id >= 0)
	{
		Vec3Graphics* vec = (Vec3Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec3Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec3;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec3Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, const Vec2Graphics& vec2)
{
	if (location.id >= 0)
	{
		Vec2Graphics* vec = (Vec2Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec2Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec2;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec2Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, float f)
{
	if (location.id >= 0)
	{
		float* val = (float*)currentGFXContext->allocateFromCommandBuffer(sizeof(float), sce::Gnm::kEmbeddedDataAlignment4);
		*val = f;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(float));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, double d)
{
	if (location.id >= 0)
	{
		double* val = (double*)currentGFXContext->allocateFromCommandBuffer(sizeof(double), sce::Gnm::kEmbeddedDataAlignment4);
		*val = d;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(double));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, int i)
{
	if (location.id >= 0)
	{
		int* val = (int*)currentGFXContext->allocateFromCommandBuffer(sizeof(int), sce::Gnm::kEmbeddedDataAlignment4);
		*val = i;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(int));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(const shaderResourceLocation& location, unsigned int u)
{
	if (location.id >= 0)
	{
		unsigned int* val = (unsigned int*)currentGFXContext->allocateFromCommandBuffer(sizeof(unsigned int), sce::Gnm::kEmbeddedDataAlignment4);
		*val = u;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(unsigned int));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
	}
}

void PS4Renderer::UpdateShaderMatrices()
{
	if (currentShader)
	{		
		shaderResourceLocation location;
		//Model Matrix in RenderComponent class.
		//Texture matrix in material class.	
		location = currentShader->GetResourceByName("viewMatrix");
		if (location.id >= 0)
		{
			Mat4Graphics* vMat = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
			*vMat = viewMatrix;
		
			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(vMat, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
		}
		location = currentShader->GetResourceByName("projMatrix");
		if (location.id >= 0)
		{
			Mat4Graphics* pMat = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
			*pMat = projMatrix;

			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(pMat, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
		}
		location = currentShader->GetResourceByName("inverseProjView"); 
		if (location.id >= 0)
		{
			Mat4Graphics* invVP = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
			*invVP = Mat4Graphics::Inverse(projMatrix * viewMatrix);

			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(invVP, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
		}
	}
}

unsigned int PS4Renderer::TextureMemoryUsage(sce::Gnm::Texture& id)
{
	uint32_t width, height;

	width = id.getWidth();
	height = id.getHeight();

	return ((width * height) * 4.0f);
}
#endif