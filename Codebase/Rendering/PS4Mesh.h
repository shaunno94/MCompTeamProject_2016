#ifdef ORBIS
#pragma once
#include "Mesh.h"
#include "Memory/PS4Memory.h"

#include <gnm.h>
#include <gnmx\context.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <gnm\dataformats.h>

using namespace sce;

class PS4Mesh : public Mesh, public PS4Memory
{
	friend class PS4RendererBase;
public:
	PS4Mesh();
	virtual ~PS4Mesh();

	void DrawDebugNormals(float length = 5.0f) override {};
	void DrawDebugTangents(float length = 5.0f) override {};

	static PS4Mesh* GenerateTriangle();
	static PS4Mesh* GenerateQuad();
	static PS4Mesh* GenerateSinglePoint();

	void Draw(Material* m) override;
	void BufferData() override;
	void SetPrimitiveType(pType type) override;

protected:
	void	SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
	void	InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset);

	//Gpu simply has a 4 byte alignment!
	struct MeshVertex
	{
		float position[3];
		float textureCoord[2];
		float normal[3];
		float tangent[3];
	};

	sce::Gnm::IndexSize		indexType;
	sce::Gnm::PrimitiveType primitiveType;

	int*		indexBuffer;
	MeshVertex*	vertexBuffer;

	int	vertexDataSize;
	int indexDataSize;

	sce::Gnm::Buffer*	attributeBuffers;
	int					attributeCount;
};
#endif