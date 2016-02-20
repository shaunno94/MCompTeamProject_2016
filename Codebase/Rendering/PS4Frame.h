#ifdef ORBIS
#pragma once
#include "Memory/PS4Memory.h"
#include <gnmx.h>

class PS4Frame : public PS4Memory
{
public:
	PS4Frame();
	~PS4Frame();

	sce::Gnmx::GnmxGfxContext& GetCommandBuffer() { return commandBuffer; }

	void BlockUntilReady();
	void StartFrame();
	void EndFrame();

protected:
	sce::Gnmx::GnmxGfxContext commandBuffer;
	uint64_t frameTag;
};
#endif
