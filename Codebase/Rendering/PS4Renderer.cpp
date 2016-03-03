#ifdef ORBIS
#include "PS4Renderer.h"
#include <video_out.h>	//Video System
#include <gnmx\basegfxcontext.h>
#include "Renderer.h"

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
	//trilinearSampler.setWrapMode(sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap, sce::Gnm::kWrapModeWrap);
	trilinearSampler.setWrapMode(sce::Gnm::kWrapModeMirror, sce::Gnm::kWrapModeMirror, sce::Gnm::kWrapModeMirror);

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

	//quad for final render
	fullScreenQuad = new GameObject();
	fullScreenQuad->SetRenderComponent(new RenderComponent(new LightMaterial(new PS4Shader(SHADER_DIR"combineVert.sb", SHADER_DIR"combineFrag.sb")), Mesh::GenerateQuad()));
	((LightMaterial*)fullScreenQuad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.EMISSIVE.name, (int)ReservedOtherTextures.EMISSIVE.index);
	((LightMaterial*)fullScreenQuad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.SPECULAR.name, (int)ReservedOtherTextures.SPECULAR.index);
	init = true;

	if (!fullScreenQuad->GetRenderComponent()->m_Material->GetShader()->IsOperational())
	{
		std::cerr << "Combine Shader is not operational" << std::endl;
		init = false;
	}

	SwapBuffers();	
}

void PS4Renderer::InitialiseVideoSystem()
{
	//open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);	
	
	G_buffer = new PS4Buffer(width, height, stackAllocators[GARLIC], 2, videoHandle, false, NONE);
	light_buffer = new PS4Buffer(width, height, stackAllocators[GARLIC], 2, videoHandle, false, NONE);

	frameBuffers.resize(MAX_BACK_BUFFER);
	frameBuffers[BACK_BUFFER1] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, videoHandle, false, FIRST_ONLY);
	frameBuffers[BACK_BUFFER2] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, videoHandle, false, FIRST_ONLY);
	frameBuffers[BACK_BUFFER3] = new PS4Buffer(width, height, stackAllocators[GARLIC], 1, videoHandle, false, FIRST_ONLY);

	for (uint i = 0; i < MAX_TARGETS_PER_BUFFER; ++i)
	{
		RegisterTargets(i);
	}
}

void PS4Renderer::RegisterTargets(const uint targetIndex)
{
	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute, SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB, SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9, width, height, (*frameBuffers[0]->GetTarget(targetIndex)).getPitch());

	void* bufferAddresses[MAX_BACK_BUFFER];
	for (int i = 0; i < MAX_BACK_BUFFER; ++i)
	{
		bufferAddresses[i] = (*frameBuffers[i]->GetTarget(targetIndex)).getBaseAddress();
	}
	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, MAX_BACK_BUFFER, &attribute);
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

	this->garlicAllocator = sce::Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC]);
	this->onionAllocator = sce::Gnmx::Toolkit::GetInterface(&stackAllocators[ONION]);
	sce::Gnm::registerOwner(&ownerHandle, "PS4Renderer");
}

void PS4Renderer::DestroyMemoryAllocators() 
{
	//stackAllocators[GARLIC].deinit();
	//stackAllocators[ONION].deinit();
}

void	PS4Renderer::DestroyGCMRendering() 
{
	//onionAllocator.release(frames);
}

void	PS4Renderer::DestroyVideoSystem() 
{
	for (int i = 0; i < MAX_BACK_BUFFER; ++i)
	{
		delete frameBuffers[i];
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

	// Filtering options
	/*if ((flags & NEAREST_NEIGHBOUR_MIN_FILTERING) == NEAREST_NEIGHBOUR_MIN_FILTERING) 
	{
		if ((flags & NEAREST_NEIGHBOUR_MAX_FILTERING) == NEAREST_NEIGHBOUR_MAX_FILTERING)
		{
			trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModePoint, sce::Gnm::kFilterModePoint);
			trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModePoint);
		}
		//mag/min filter
		trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModePoint);
		trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModePoint);
	}
	else if ((flags & BILINEAR_MIN_FILTERING) == BILINEAR_MIN_FILTERING) 
	{
		if ((flags & BILINEAR_MAX_FILTERING) == BILINEAR_MAX_FILTERING) 
		{
			trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModeAnisoBilinear);
			trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModePoint);
		}
		trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModeAnisoBilinear);
		trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModePoint);
	}
	else if ((flags & TRILINEAR_MIN_FILTERING) == TRILINEAR_MIN_FILTERING) {
		trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModeAnisoBilinear);
		trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	}


	else if ((flags & TRILINEAR_MAX_FILTERING) == TRILINEAR_MAX_FILTERING) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}*/

	/*if ((flags & ANISOTROPIC_FILTERING) == ANISOTROPIC_FILTERING) {
		trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeAnisoBilinear, sce::Gnm::kFilterModeAnisoBilinear);
		trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	}*/
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
}

void PS4Renderer::SetCurrentShader(BaseShader* s)
{	
	if (s != currentShader)
		currentShader = static_cast<PS4Shader*>(s);

	currentShader->SubmitShaderSwitch(*currentGFXContext);
}

void PS4Renderer::FillBuffers()
{
	currentFrame->StartFrame();
	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);
	
	G_buffer->SetRenderTargets(*currentGFXContext);
	InitCMD();

	child->OnRenderScene();
	
	//Albedo texture
	G_buffer->GetTexture(COLOUR)->initFromRenderTarget(G_buffer->GetTarget(0), false);
	G_buffer->GetTexture(COLOUR)->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	//Normal texture
	G_buffer->GetTexture(COLOUR + 1)->initFromRenderTarget(G_buffer->GetTarget(1), false);
	G_buffer->GetTexture(COLOUR + 1)->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	//Depth texture
	G_buffer->GetTexture(DEPTH)->initFromDepthRenderTarget(G_buffer->GetDepthTarget(), false);
	G_buffer->GetTexture(DEPTH)->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
	sce::Gnmx::decompressDepthSurface(currentGFXContext, G_buffer->GetDepthTarget());
}

void PS4Renderer::InitCMD()
{
	currentGFXContext->setDbRenderControl(dbRenderControl);
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
	currentGFXContext->setDepthStencilControl(dsc);
	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->GetBufferWidth(), currentPS4Buffer->GetBufferHeight(), 0.5f, 0.5f);
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
	currentGFXContext->setBlendControl(0, blendControl);
	currentGFXContext->setDepthEqaaControl(dEqaaControl);
	currentGFXContext->setAaSampleCount(sce::Gnm::NumSamples::kNumSamples4, 100);
	currentGFXContext->setClipControl(cc);
}

void PS4Renderer::DrawPointLights()
{
	light_buffer->SetRenderTargets(*currentGFXContext);

	blendControl.setAlphaEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOne);
	InitCMD();

	PS4ShaderResourceLocations depthLoc;
	depthLoc.id = 0;
	SetTexture(depthLoc, *G_buffer->GetTexture(DEPTH));

	PS4ShaderResourceLocations NormalLoc;
	NormalLoc.id = 1;
	SetTexture(NormalLoc, *G_buffer->GetTexture(COLOUR + 1));
	
	child->OnRenderLights();

	//Emissive texture
	light_buffer->GetTexture(COLOUR)->initFromRenderTarget(light_buffer->GetTarget(0), false);
	light_buffer->GetTexture(COLOUR)->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	//Specular texture
	light_buffer->GetTexture(COLOUR + 1)->initFromRenderTarget(light_buffer->GetTarget(1), false);
	light_buffer->GetTexture(COLOUR + 1)->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
	
	primitiveSetup.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	blendControl.setColorEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOneMinusSrcAlpha);
}

void PS4Renderer::CombineBuffers()
{
	currentPS4Buffer->SetRenderTargets(*currentGFXContext);

	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, 1, -1);
	InitCMD();

	PS4ShaderResourceLocations diffuseLoc;
	diffuseLoc.id = 0;
	SetTexture(diffuseLoc, *G_buffer->GetTexture(COLOUR));

	PS4ShaderResourceLocations emissiveLoc;
	emissiveLoc.id = 1;
	SetTexture(emissiveLoc, *light_buffer->GetTexture(COLOUR));

	PS4ShaderResourceLocations specularLoc;
	specularLoc.id = 2;
	SetTexture(specularLoc, *light_buffer->GetTexture(COLOUR + 1));

	fullScreenQuad->GetRenderComponent()->Draw();
	currentFrame->EndFrame();
}

void PS4Renderer::DrawShadow(GameObject* light)
{
	LightMaterial* lm = (LightMaterial*)(light->GetRenderComponent()->m_Material);
	switch (lm->shadowType)
	{
	case _NONE:
	default:
		break;
	case _2D:
		DrawShadow2D(light);
		break;
	case _CUBE:
		DrawShadowCube(light);
		break;
	}
}

void PS4Renderer::DrawShadow2D(GameObject* light)
{

}

void PS4Renderer::DrawShadowCube(GameObject* light)
{

}

//TODO:fix id here
void PS4Renderer::SetTexture(const shaderResourceLocation& location, textureHandle& handle)
{
	currentGFXContext->setTextures(sce::Gnm::kShaderStagePs, location.id, 1, &handle);		
}

void PS4Renderer::SwapBuffers()
{
	SwapScreenBuffer();
	SwapCommandBuffer();
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
	
	currentPS4Buffer = frameBuffers[currentScreenBuffer];
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
			*invVP = Matrix4Simple::Inverse(projMatrix * viewMatrix);

			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(invVP, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(location.stage, location.id, 1, &constantBuffer);
		}
	}
}
#endif