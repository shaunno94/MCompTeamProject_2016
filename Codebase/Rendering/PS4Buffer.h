#ifdef ORBIS
#pragma once
#include <gnm.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4Shader.h"

class PS4Buffer : PS4Memory
{
public:
	PS4Buffer(const uint width, const uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, const uint numRTargets, const int videoHandle, bool genStencil);
	void ClearBuffer(sce::Gnmx::GnmxGfxContext& context);
	void SetRenderTargets(sce::Gnmx::GnmxGfxContext& context);

	uint32_t GetBufferHeight() const { return height; }
	uint32_t GetBufferWidth() const  { return width; }

	sce::Gnm::RenderTarget& GetTarget(uint i) { return buffer->renderTargets[i]; }

	~PS4Buffer();
	
private:
	void GenerateDepthTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator);
	void GenerateRenderTarget(uint width, uint height, sce::Gnmx::Toolkit::StackAllocator& allocator, uint targetID, bool displayable);	
	void RegisterTargets(const int videoHandle);

	std::vector<sce::Gnm::Texture> textures;
	uint numRenderTargets;
	uint32_t width, height;
	bool stencil;

	static unsigned int ID;

	struct ScreenBuffer
	{
		std::vector<sce::Gnm::RenderTarget> renderTargets;
		sce::Gnm::DepthRenderTarget depthTarget;
	};

	ScreenBuffer* buffer;
};
#endif