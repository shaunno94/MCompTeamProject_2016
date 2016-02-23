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

	currentScreenBuffer = 0;
	currentGFXContext = nullptr;

	InitialiseMemoryAllocators();
	InitialiseGCMRendering();
	InitialiseVideoSystem();

	SwapBuffers();
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

}

void	PS4Renderer::DestroyGCMRendering() 
{
	//onionAllocator->release(frames);
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
	currentShader = static_cast<PS4Shader*>(s);
}

void PS4Renderer::FillBuffers()
{
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);
	SetRenderBuffer(currentPS4Buffer, true, true, true);

	//defaultShader->SubmitShaderSwitch(*currentGFXContext);

	//Primitive Setup State
	sce::Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(sce::Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	//Screen Access State
	sce::Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(sce::Gnm::kDepthControlZWriteEnable, sce::Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	currentGFXContext->setDepthStencilControl(dsc);
	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), 0.5f, 0.5f);

	sce::Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);

	//trilinearSampler.set
	trilinearSampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);

	//currentGFXContext->setTextures(sce::Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	//currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	child->OnRenderScene();

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

void PS4Renderer::ClearBuffer(bool colour, bool depth, bool stencil)
{
	if (colour) 
	{
		Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
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
	if (currentGFXContext) {
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) {
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

}
#endif