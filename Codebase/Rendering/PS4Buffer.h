#ifdef ORBIS
#pragma once
#include <gnm.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4Shader.h"

enum TextureIndex
{
	DEPTH, COLOUR
};

enum Displayable
{
	NONE, FIRST_ONLY
};

class PS4Buffer : PS4Memory
{
public:
	PS4Buffer(const uint width, const uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, const uint numRTargets, bool genStencil, Displayable display);
	void ClearBuffer(sce::Gnmx::GnmxGfxContext& context);
	void SetRenderTargets(sce::Gnmx::GnmxGfxContext& context);

	uint32_t GetBufferHeight() const { return height; }
	uint32_t GetBufferWidth() const  { return width; }
	static uint64_t GetFBOMemUsage() { return FBOMemUsage; }

	sce::Gnm::RenderTarget* GetTarget(uint i) { return i > numRenderTargets ? nullptr : &buffer->renderTargets[i]; }
	sce::Gnm::DepthRenderTarget* GetDepthTarget() { return &buffer->depthTarget; }
	sce::Gnm::Texture* GetTexture(uint i) { return i > (numRenderTargets + 1) ? nullptr : &textures[i]; }

	~PS4Buffer();
	
private:
	void GenerateDepthTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator);
	void GenerateRenderTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, uint targetID, Displayable display);
	void CalcInternalTexUsage(sce::Gnm::Texture& handle);

	std::vector<sce::Gnm::Texture> textures;
	uint numRenderTargets;
	uint32_t width, height;
	bool stencil;

	static unsigned int ID;
	static uint64_t FBOMemUsage;

	struct ScreenBuffer
	{
		std::vector<sce::Gnm::RenderTarget> renderTargets;
		sce::Gnm::DepthRenderTarget depthTarget;
	};

	ScreenBuffer* buffer;
};
#endif