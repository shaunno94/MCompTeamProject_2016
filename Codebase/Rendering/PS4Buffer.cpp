#ifdef ORBIS
#include "PS4Buffer.h"
#include <video_out.h>

unsigned int PS4Buffer::ID = 0;

PS4Buffer::PS4Buffer(const uint width, const uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, const uint numRTargets, const int videoHandle, bool genStencil)
{
	buffer = new ScreenBuffer();
	buffer->renderTargets.resize(numRTargets);
	textures.resize(numRTargets + 1);

	for (uint i = 0; i < numRTargets; ++i)
	{
		GenerateRenderTarget(width, height, allocator, i, i > 0 ? false : true);
	}

	GenerateDepthTarget(width, height, allocator);

	numRenderTargets = numRTargets;
	this->width = width;
	this->height = height;
	stencil = genStencil;
	ID++;
	
	//RegisterTargets(videoHandle);
}

PS4Buffer::~PS4Buffer()
{
	delete buffer;
}

void PS4Buffer::RegisterTargets(const int videoHandle)
{
	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute, SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB, SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9, buffer->renderTargets[0].getWidth(), buffer->renderTargets[0].getHeight(), buffer->renderTargets[0].getPitch());

	void* bufferAddresses[numRenderTargets];
	for (int i = 0; i < numRenderTargets; ++i)
	{
		bufferAddresses[i] = buffer->renderTargets[i].getBaseAddress();
	}
	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, numRenderTargets, &attribute);
}

void PS4Buffer::GenerateRenderTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, uint targetID, bool displayable)
{
	sce::Gnm::DataFormat format = sce::Gnm::kDataFormatB8G8R8A8UnormSrgb;
	sce::Gnm::TileMode	tileMode;
	sce::GpuAddress::computeSurfaceTileMode(&tileMode, displayable ? sce::GpuAddress::kSurfaceTypeColorTargetDisplayable : sce::GpuAddress::kSurfaceTypeColorTarget, format, 1);

	const sce::Gnm::SizeAlign colourAlign = buffer->renderTargets[targetID].init(width, height, 1, format, tileMode, sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1, nullptr, nullptr);
	void* colourMemory = allocator.allocate(colourAlign);

	sce::Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
		"Target" + *std::to_string(ID).c_str(), sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

	buffer->renderTargets[targetID].setAddresses(colourMemory, nullptr, nullptr);
}

void PS4Buffer::GenerateDepthTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator)
{
	//Depth Buffer
	sce::Gnm::DataFormat depthFormat = sce::Gnm::DataFormat::build(sce::Gnm::kZFormat32Float);
	sce::Gnm::TileMode	depthTileMode;

	sce::GpuAddress::computeSurfaceTileMode(&depthTileMode, sce::GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);
	sce::Gnm::StencilFormat stencilFormat = (sce::Gnm::kStencil8);

	void* stencilMemory = 0;
	sce::Gnm::SizeAlign stencilAlign;

	const sce::Gnm::SizeAlign depthAlign = buffer->depthTarget.init(width, height, depthFormat.getZFormat(), stencilFormat, depthTileMode,
		sce::Gnm::kNumFragments1, &stencilAlign, 0);
	void* depthMemory = allocator.allocate(depthAlign);

	sce::Gnm::registerResource(nullptr, ownerHandle, depthMemory, depthAlign.m_size,
		"Depth", sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
	
	//Stencil buffer
	if (stencil)
	{
		stencilMemory = allocator.allocate(stencilAlign);
		sce::Gnm::registerResource(nullptr, ownerHandle, stencilMemory, stencilAlign.m_size,
		"Stencil", sce::Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
	}
	buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
}

void PS4Buffer::ClearBuffer(sce::Gnmx::GnmxGfxContext& context)
{
	Vector4 defaultClearColour(0.0f, 0.0f, 0.0f, 1.0f);
	for (uint i = 0; i < numRenderTargets; ++i)
	{
		sce::Gnmx::Toolkit::SurfaceUtil::clearRenderTarget(context, &buffer->renderTargets[i], defaultClearColour);
	}
	sce::Gnmx::Toolkit::SurfaceUtil::clearDepthTarget(context, &buffer->depthTarget, 1.0f);
	
	if (stencil)
		sce::Gnmx::Toolkit::SurfaceUtil::clearStencilTarget(context, &buffer->depthTarget, 0);
}

void PS4Buffer::SetRenderTargets(sce::Gnmx::GnmxGfxContext& context)
{
	ClearBuffer(context);

	context.setRenderTargetMask(0xFFFFF);
	for (uint i = 0; i < numRenderTargets; ++i)
	{
		context.setRenderTarget(i, &buffer->renderTargets[i]);
	}	
	context.setDepthRenderTarget(&buffer->depthTarget);
}
#endif