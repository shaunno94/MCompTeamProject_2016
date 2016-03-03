#ifdef ORBIS
#include "PS4Renderer.h"
#include <video_out.h>	//Video System
#include <gnmx\basegfxcontext.h>
#include "Renderer.h"

Renderer* PS4Renderer::child = nullptr;

PS4Renderer::PS4Renderer()
{
	currentGPUBuffer = 0;
	currentScreenBuffer = 0;
	prevScreenBuffer = 0;
	projMatrix.ToIdentity();
	viewMatrix.ToIdentity();

	currentScreenBuffer = 0;
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
	dsc.setDepthControl(sce::Gnm::kDepthControlZWriteEnable, sce::Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	
	blendControl.init();
	blendControl.setBlendEnable(true);
	blendControl.setColorEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOneMinusSrcAlpha);

	dEqaaControl.init();
	dEqaaControl.setAlphaToMaskSamples(sce::Gnm::NumSamples::kNumSamples4);

	cc.init();

	SwapBuffers();	
	init = true;
}

void PS4Renderer::InitialiseVideoSystem()
{
	//open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);	
	
	TestBuf.resize(_bufferCount);
	for (int i = 0; i < _bufferCount; ++i)
	{
		TestBuf[i] = new PS4Buffer(1920, 1080, stackAllocators[GARLIC], 2, videoHandle, false);
	}

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		TestBuf[0]->GetBufferWidth(),
		TestBuf[0]->GetBufferHeight(),
		TestBuf[0]->GetTarget(0).getPitch());

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i)
	{
		bufferAddresses[i] = TestBuf[i]->GetTarget(0).getBaseAddress();
	}
	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);

	for (int i = 0; i < _bufferCount; ++i)
	{
		bufferAddresses[i] = TestBuf[i]->GetTarget(1).getBaseAddress();
	}
	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);
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
	for (int i = 0; i < _bufferCount; ++i) 
	{
		//delete screenBuffers[i];
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
	
	currentPS4Buffer->SetRenderTargets(*currentGFXContext);
		
	InitCMD();

	child->OnRenderScene();

	/*currentPS4Buffer->normalTex.initFromRenderTarget(&currentPS4Buffer->normalTarget, false);
	currentPS4Buffer->normalTex.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	currentPS4Buffer->depthTex.initFromDepthRenderTarget(&currentPS4Buffer->depthTarget, false);
	currentPS4Buffer->depthTex.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
	sce::Gnmx::decompressDepthSurface(currentGFXContext, &currentPS4Buffer->depthTarget);*/
	currentFrame->EndFrame();
}

void PS4Renderer::InitCMD()
{
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
	//currentFrame->StartFrame();
	//currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);
	//SetRenderBuffer(currentPS4Buffer);

	//blendControl.setAlphaEquation(sce::Gnm::kBlendMultiplierOne, sce::Gnm::kBlendFuncAdd, sce::Gnm::kBlendMultiplierOne);
	//InitCMD();
	////SetTexture();
	//sce::Gnm::Texture depthTex;
	//depthTex.initFromDepthRenderTarget(&currentPS4Buffer->depthTarget, false);
	//
	//child->OnRenderLights();

	//currentFrame->EndFrame();
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

void PS4Renderer::CombineBuffers()
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
	SwapCommandBuffer(true);
}

void PS4Renderer::SwapCommandBuffer(bool submitDone)
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
	currentScreenBuffer = (currentScreenBuffer + 1) % _bufferCount;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);
	
	currentPS4Buffer = TestBuf[currentScreenBuffer];
}

void PS4Renderer::SetCullFace(CULL c)
{
	primitiveSetup.setCullFace(c == FRONT ? sce::Gnm::kPrimitiveSetupCullFaceFront : sce::Gnm::kPrimitiveSetupCullFaceBack);
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

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location.id, 1, &constantBuffer);
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

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location.id, 1, &constantBuffer);
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

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location.id, 1, &constantBuffer);
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

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location.id, 1, &constantBuffer);
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

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location.id, 1, &constantBuffer);
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
	}
}
#endif