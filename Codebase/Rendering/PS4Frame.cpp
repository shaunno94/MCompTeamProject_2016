#include "PS4Frame.h"
#include <gnmx\basegfxcontext.h>

enum FrameState 
{
	FRAME_READY,
	FRAME_WAITING,
	FRAME_DONE
};

PS4Frame::PS4Frame()
{
	frameTag = FRAME_READY;

	//Need to allocate memory for our command buffer
	const int bufferBytes = (1 * 1024 * 1024);

	const uint32_t kNumRingEntries = 64;
	const uint32_t cueHeapSize = sce::Gnmx::ConstantUpdateEngine::computeHeapSize(kNumRingEntries);
	void *constantUpdateEngine = garlicAllocator.allocate(cueHeapSize, sce::Gnm::kAlignmentOfBufferInBytes);
	void *drawCommandBuffer = onionAllocator.allocate(bufferBytes, sce::Gnm::kAlignmentOfBufferInBytes);
	void *constantCommandBuffer = onionAllocator.allocate(bufferBytes, sce::Gnm::kAlignmentOfBufferInBytes);

	commandBuffer.init(constantUpdateEngine, kNumRingEntries, drawCommandBuffer, bufferBytes, constantCommandBuffer, bufferBytes);

	sce::Gnm::registerResource(nullptr, ownerHandle, drawCommandBuffer, bufferBytes,
		"FrameDrawCommandBuffer", sce::Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
	sce::Gnm::registerResource(nullptr, ownerHandle, constantUpdateEngine, bufferBytes,
		"FrameConstantUpdateEngine", sce::Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
	sce::Gnm::registerResource(nullptr, ownerHandle, constantCommandBuffer, bufferBytes,
		"FrameConstantCommandBuffer", sce::Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
}

PS4Frame::~PS4Frame() {}

void PS4Frame::StartFrame()
{
	BlockUntilReady();
	frameTag = FRAME_WAITING;

	commandBuffer.reset();
	commandBuffer.initializeDefaultHardwareState();
}

void PS4Frame::BlockUntilReady() 
{
	switch (frameTag)
	{
		case FRAME_READY:	return;
		case FRAME_DONE:	return;

		case FRAME_WAITING: 
		{
			int spinCount = 0;
			while (frameTag != FRAME_DONE) { ++spinCount; }
			frameTag = FRAME_READY;
		}
		break;
	default:
		break;
	}
}

void  PS4Frame::EndFrame() 
{
	commandBuffer.writeImmediateAtEndOfPipeWithInterrupt(sce::Gnm::kEopFlushCbDbCaches, &frameTag, FRAME_DONE, sce::Gnm::kCacheActionNone);
}