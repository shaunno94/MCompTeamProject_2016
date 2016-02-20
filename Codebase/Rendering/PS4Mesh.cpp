#ifdef ORBIS
#include "PS4Mesh.h"

PS4Mesh::PS4Mesh(size_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, size_t numIndices, size_t* indices)
: Mesh(numVertices, vertices, texCoords, normals, tangents, numIndices, indices)
{
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;
	indexType = sce::Gnm::IndexSize::kIndexSize32;
}

PS4Mesh::PS4Mesh()
{
	indexBuffer = 0;
	vertexBuffer = 0;
	attributeCount = 0;
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;
	indexType = sce::Gnm::IndexSize::kIndexSize32;
}

PS4Mesh::~PS4Mesh()
{
	delete[] attributeBuffers;
}

void PS4Mesh::SetPrimitiveType(pType type)
{
	switch (type)
	{
	case TRIANGLE:
		this->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypePolygon;
		break;
	case TRIANGLE_STRIP:
		this->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
		break;
	}
	indexType = sce::Gnm::IndexSize::kIndexSize32;
}

void PS4Mesh::BufferData() 
{
	vertexDataSize = m_NumVertices * sizeof(MeshVertex);
	indexDataSize = m_NumIndices * sizeof(int);

	indexBuffer = static_cast<int*>			(garlicAllocator.allocate(indexDataSize, sce::Gnm::kAlignmentOfBufferInBytes));
	vertexBuffer = static_cast<MeshVertex*>	(garlicAllocator.allocate(vertexDataSize, sce::Gnm::kAlignmentOfBufferInBytes));

	sce::Gnm::registerResource(nullptr, ownerHandle, indexBuffer, indexDataSize, "IndexData", sce::Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	sce::Gnm::registerResource(nullptr, ownerHandle, vertexBuffer, vertexDataSize, "VertexData", sce::Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	for (unsigned int i = 0; i < m_NumVertices; ++i)
	{
		memcpy(&vertexBuffer[i].position, &m_Vertices[i], sizeof(float)* 3);
		memcpy(&vertexBuffer[i].textureCoord, &m_TextureCoords[i], sizeof(float)* 2);
		memcpy(&vertexBuffer[i].normal, &m_Normals[i], sizeof(float)* 3);
		memcpy(&vertexBuffer[i].tangent, &m_Tangents[i], sizeof(float)* 3);
	}

	for (unsigned int i = 0; i < m_NumIndices; ++i) 
	{ //Our index buffer might not have the same data size as the source indices?
		indexBuffer[i] = m_Indices[i];
	}

	attributeCount = 4;
	attributeBuffers = new sce::Gnm::Buffer[4];

	InitAttributeBuffer(attributeBuffers[0], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], sce::Gnm::kDataFormatR32G32Float, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].tangent));
}

void PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer &buffer, sce::Gnm::DataFormat format, void*offset)
{
	int lolz = sizeof(MeshVertex);
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), m_NumVertices);
	buffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
}

void PS4Mesh::Draw(Material* m)
{

}

void PS4Mesh::SubmitDraw(sce::Gnmx::GnmxGfxContext& cmdList, sce::Gnm::ShaderStage stage)
{
	cmdList.setVertexBuffers(stage, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);
	cmdList.drawIndex(m_NumIndices, indexBuffer);
}
#endif