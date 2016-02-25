#ifdef ORBIS
#include "PS4Renderer.h"
#include <video_out.h>	//Video System
#include <gnmx\basegfxcontext.h>
#include "Renderer.h"

Renderer* PS4Renderer::child = nullptr;

PS4Renderer::PS4Renderer()
{
	framesSubmitted = 0;
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

	primitiveSetup.init();
	primitiveSetup.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(sce::Gnm::kPrimitiveSetupFrontFaceCcw);

	dsc.init();
	dsc.setDepthControl(sce::Gnm::kDepthControlZWriteEnable, sce::Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);

	mesh = static_cast<PS4Mesh*>(Mesh::GenerateQuad());
	shader = new PS4Shader(SHADER_DIR"textureVertex.sb", SHADER_DIR"textureFragment.sb");

	SwapBuffers();	
	init = true;
}

void PS4Renderer::InitialiseVideoSystem()
{
	screenBuffers = new PS4ScreenBuffer*[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) 
	{
		screenBuffers[i] = GenerateScreenBuffer(1920, 1080);
	}

	//Now we can open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		screenBuffers[0]->colourTarget.getWidth(),
		screenBuffers[0]->colourTarget.getHeight(),
		screenBuffers[0]->colourTarget.getPitch());

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) 
	{
		bufferAddresses[i] = screenBuffers[i]->colourTarget.getBaseAddress();
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
		delete screenBuffers[i];
	}
	delete[] screenBuffers;
	sceVideoOutClose(videoHandle);
}

PS4Renderer::~PS4Renderer()
{
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void PS4Renderer::SetTextureFlags(const sce::Gnm::Texture*, unsigned int flags)
{

}

void PS4Renderer::SetCurrentShader(BaseShader* s)
{	
	if (s == currentShader)
		return;

	currentShader = static_cast<PS4Shader*>(s);
	currentShader->SubmitShaderSwitch(*currentGFXContext);
}

void PS4Renderer::FillBuffers()
{
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	//shader->SubmitShaderSwitch(*currentGFXContext);

	SetRenderBuffer(currentPS4Buffer, true, true, true);
	
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
	currentGFXContext->setDepthStencilControl(dsc);
	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), 0.5f, 0.5f);
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	child->OnRenderScene();

	//UpdateShaderMatrices();
	//UpdateUniform(shader->GetModelMatrixLocation(), Matrix4Simple::Translation(Vector3Simple(0, 0, 0)));
	//mesh->Draw(nullptr);


	currentFrame->EndFrame();
	framesSubmitted++;
}

void PS4Renderer::DrawPointLights()
{

}

void PS4Renderer::DrawShadow(GameObject* light)
{

}

void PS4Renderer::CombineBuffers()
{

}

void PS4Renderer::SetTexture(unsigned int id, textureHandle handle)
{
	currentGFXContext->setTextures(sce::Gnm::kShaderStagePs, id, 1, handle);
}

void PS4Renderer::ClearBuffer(bool colour, bool depth, bool stencil)
{
	if (colour) 
	{
		Vector4 defaultClearColour(1.0f, 1.0f, 0.0f, 1.0f);
		sce::Gnmx::Toolkit::SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, defaultClearColour);
	}
	if (depth) 
	{
		sce::Gnmx::Toolkit::SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, 1.0f);
	}

	if (stencil && currentPS4Buffer->depthTarget.getStencilReadAddress()) 
	{
		sce::Gnmx::Toolkit::SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, 0);
	}
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
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) 
		{
			bool a = true;
		}
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
	
	currentPS4Buffer = screenBuffers[currentScreenBuffer];
}

void PS4Renderer::SetRenderBuffer(PS4ScreenBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil) 
{
	currentPS4Buffer = buffer;

	ClearBuffer(clearColour, clearDepth, clearStencil);

	currentGFXContext->setRenderTargetMask(0xF);
	currentGFXContext->setRenderTarget(0, &currentPS4Buffer->colourTarget);
	currentGFXContext->setDepthRenderTarget(&currentPS4Buffer->depthTarget);
}

PS4ScreenBuffer* PS4Renderer::GenerateScreenBuffer(uint width, uint height, bool colour, bool depth, bool stencil)
{
	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	if (colour)
	{
		sce::Gnm::DataFormat format = sce::Gnm::kDataFormatB8G8R8A8UnormSrgb;
		sce::Gnm::TileMode	tileMode;
		sce::GpuAddress::computeSurfaceTileMode(&tileMode, sce::GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1);

		const sce::Gnm::SizeAlign colourAlign = buffer->colourTarget.init(width, height, 1, format, tileMode,
			sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1, NULL, NULL);

		void *colourMemory = stackAllocators[GARLIC].allocate(colourAlign);

		sce::Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
			"Colour", sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		buffer->colourTarget.setAddresses(colourMemory, NULL, NULL);
	}

	if (depth)
	{
		sce::Gnm::DataFormat depthFormat = sce::Gnm::DataFormat::build(sce::Gnm::kZFormat32Float);
		sce::Gnm::TileMode	depthTileMode;

		sce::GpuAddress::computeSurfaceTileMode(&depthTileMode, sce::GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);

		sce::Gnm::StencilFormat stencilFormat = (stencil ? sce::Gnm::kStencil8 : sce::Gnm::kStencilInvalid);

		void* stencilMemory = 0;
		sce::Gnm::SizeAlign stencilAlign;

		const sce::Gnm::SizeAlign depthAlign = buffer->depthTarget.init(width, height, depthFormat.getZFormat(), stencilFormat, depthTileMode,
			sce::Gnm::kNumFragments1, stencil ? &stencilAlign : 0, 0);

		void *depthMemory = stackAllocators[GARLIC].allocate(depthAlign);

		sce::Gnm::registerResource(nullptr, ownerHandle, depthMemory, depthAlign.m_size,
			"Depth", sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);


		if (stencil) 
		{
			stencilMemory = stackAllocators[GARLIC].allocate(stencilAlign);

			sce::Gnm::registerResource(nullptr, ownerHandle, stencilMemory, stencilAlign.m_size,
				"Stencil", sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
		}

		buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
	}
	return buffer;
}

void PS4Renderer::SetCullFace(CULL c)
{
	primitiveSetup.setCullFace(c == FRONT ? sce::Gnm::kPrimitiveSetupCullFaceFront : sce::Gnm::kPrimitiveSetupCullFaceBack);
}

void PS4Renderer::UpdateUniform(int location, const Mat4Graphics& mat4)
{
	if (location >= 0)
	{
		Mat4Graphics* matrix = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*matrix = mat4;
		sce::Gnm::Buffer constantBuffer; 
		constantBuffer.initAsConstantBuffer(matrix , sizeof(Mat4Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, const Mat3Graphics& mat3)
{
	if (location >= 0)
	{
		Mat3Graphics* matrix = (Mat3Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat3Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*matrix = mat3;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(matrix, sizeof(Mat3Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, const Vec4Graphics& vec4)
{
	if (location >= 0)
	{
		Vec4Graphics* vec = (Vec4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec4;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec4Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, const Vec3Graphics& vec3)
{
	if (location >= 0)
	{
		Vec3Graphics* vec = (Vec3Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec3Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec3;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec3Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, const Vec2Graphics& vec2)
{
	if (location >= 0)
	{
		Vec2Graphics* vec = (Vec2Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vec2Graphics), sce::Gnm::kEmbeddedDataAlignment4);
		*vec = vec2;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(vec, sizeof(Vec2Graphics));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, float f)
{
	if (location >= 0)
	{
		float* val = (float*)currentGFXContext->allocateFromCommandBuffer(sizeof(float), sce::Gnm::kEmbeddedDataAlignment4);
		*val = f;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(float));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, double d)
{
	if (location >= 0)
	{
		double* val = (double*)currentGFXContext->allocateFromCommandBuffer(sizeof(double), sce::Gnm::kEmbeddedDataAlignment4);
		*val = d;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(double));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, int i)
{
	if (location >= 0)
	{
		int* val = (int*)currentGFXContext->allocateFromCommandBuffer(sizeof(int), sce::Gnm::kEmbeddedDataAlignment4);
		*val = i;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(int));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}
void PS4Renderer::UpdateUniform(int location, unsigned int u)
{
	if (location >= 0)
	{
		unsigned int* val = (unsigned int*)currentGFXContext->allocateFromCommandBuffer(sizeof(unsigned int), sce::Gnm::kEmbeddedDataAlignment4);
		*val = u;
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(val, sizeof(unsigned int));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
	}
}

void PS4Renderer::UpdateShaderMatrices()
{
	if (currentShader)
	{		
		int location = -1;
		//Model Matrix in RenderComponent class.
		//Texture matrix in material class.	
		location = currentShader->GetResourceByName("viewMatrix");

		if (location >= 0)
		{
			Mat4Graphics* vMat = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
			*vMat = viewMatrix;
		
			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(vMat, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
		}
		
		location = currentShader->GetResourceByName("projMatrix");
		if (location >= 0)
		{
			Mat4Graphics* pMat = (Mat4Graphics*)currentGFXContext->allocateFromCommandBuffer(sizeof(Mat4Graphics), sce::Gnm::kEmbeddedDataAlignment4);
			*pMat = projMatrix;

			sce::Gnm::Buffer constantBuffer;
			constantBuffer.initAsConstantBuffer(pMat, sizeof(Mat4Graphics));
			constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
			currentGFXContext->setConstantBuffers(sce::Gnm::kShaderStageVs, location, 1, &constantBuffer);
		}
	}
}
#endif