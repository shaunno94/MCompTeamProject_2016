#ifdef ORBIS
#include "DebugDrawPS4.h"
#include "PS4Shader.h"

sce::Gnm::PrimitiveType DebugDrawPS4::primitiveType = sce::Gnm::kPrimitiveTypeLineList;
sce::Gnm::ShaderStage DebugDrawPS4::stage = sce::Gnm::kShaderStageVs;
sce::Gnm::IndexSize	DebugDrawPS4::indexType = sce::Gnm::IndexSize::kIndexSize32;
sce::Gnm::Buffer DebugDrawPS4::buffer;
PS4Shader* DebugDrawPS4::debugShader = nullptr;
const char* DebugDrawPS4::dFrag = SHADER_DIR"DebugFrag.sb";
const char* DebugDrawPS4::dVertex = SHADER_DIR"DebugVertex.sb";
btVector3* DebugDrawPS4::vertexBuffer = nullptr;
uint32_t* DebugDrawPS4::indexBuffer = nullptr;

void DebugDrawPS4::Instance()
{
	debugShader = new PS4Shader(dVertex, dFrag);
	vertexBuffer = static_cast<btVector3*>	(meshGarlicAllocator.allocate(sizeof(btVector3) * 10000, sce::Gnm::kAlignmentOfBufferInBytes));
	indexBuffer = static_cast<uint32_t*>	(meshGarlicAllocator.allocate(sizeof(uint32_t) * 10000, sce::Gnm::kAlignmentOfBufferInBytes));
}

void DebugDrawPS4::Release()
{
	delete debugShader;
	meshGarlicAllocator.release(vertexBuffer);
	meshGarlicAllocator.release(indexBuffer);
}

void DebugDrawPS4::DrawDebugLines(std::vector<btVector3>& lines, Renderer* context)
{
	for (unsigned int i = 0; i < lines.size(); ++i)
	{
		memcpy(&vertexBuffer[i], lines[i], sizeof(btVector3));
	}
	for (unsigned int i = 0; i < lines.size(); ++i)
	{
		indexBuffer[i] = i;
	}

	buffer.initAsVertexBuffer(vertexBuffer, sce::Gnm::kDataFormatR32G32B32Float, sizeof(btVector3), lines.size());
	buffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	context->SetCurrentShader(debugShader);
	context->UpdateShaderMatrices();

	context->GetGFXContext()->setVertexBuffers(stage, 0, 1, &buffer);
	context->GetGFXContext()->setPrimitiveType(primitiveType);
	context->GetGFXContext()->setIndexSize(indexType);
	context->GetGFXContext()->drawIndex(lines.size(), indexBuffer);
}
#endif