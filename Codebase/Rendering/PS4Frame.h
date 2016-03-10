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
	static uint64_t GetCMDMemUsage() { return CMDMemUsage; }

	void BlockUntilReady();
	void StartFrame();
	void EndFrame();

protected:
	sce::Gnmx::GnmxGfxContext commandBuffer;
	uint64_t frameTag;
	static uint64_t CMDMemUsage;
};
#endif
