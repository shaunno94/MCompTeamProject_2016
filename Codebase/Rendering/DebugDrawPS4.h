#ifdef ORBIS
#pragma once
#include "DebugDraw.h"
#include "Memory\PS4Memory.h"

class DebugDrawPS4 : PS4Memory
{
	friend class DebugDraw;
public:

private:
	DebugDrawPS4() {}
	~DebugDrawPS4() {}

	static void Instance();
	static void Release();
	static void DrawDebugLines(std::vector<btVector3>& lines, Renderer* context);

	static sce::Gnm::Buffer buffer;
	static sce::Gnm::PrimitiveType primitiveType;
	static sce::Gnm::IndexSize	indexType;
	static sce::Gnm::ShaderStage stage;
	static const char* dVertex;
	static const char* dFrag;
	static PS4Shader* debugShader;
	static btVector3* vertexBuffer;
	static uint32_t* indexBuffer;
};
#endif