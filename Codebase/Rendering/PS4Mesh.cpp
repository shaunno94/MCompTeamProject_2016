#ifdef ORBIS
#include "PS4Mesh.h"
#include "Renderer.h"
#include "Material.h"

PS4Mesh::PS4Mesh(uint32_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, uint32_t numIndices, uint32_t* indices)
: Mesh(numVertices, vertices, texCoords, normals, tangents, numIndices, indices)
{
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
	indexType = sce::Gnm::IndexSize::kIndexSize32;
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	attributeBuffers = nullptr;
	attributeCount = 0;
	vertexDataSize = 0;
	indexDataSize = 0;
}

PS4Mesh::PS4Mesh()
{
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	attributeBuffers = nullptr;
	attributeCount = 0;
	vertexDataSize = 0;
	indexDataSize = 0;
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
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
		primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypePolygon;
		break;
	case TRIANGLE_STRIP:
		primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;
		break;
	case TRIANGLE_LIST:
		primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
		break;
	}
	indexType = sce::Gnm::IndexSize::kIndexSize32;
}

void PS4Mesh::BufferData() 
{
	vertexDataSize = m_NumVertices * sizeof(MeshVertex);
	indexDataSize = m_NumIndices * sizeof(uint32_t);

	indexBuffer = static_cast<uint32_t*>	(garlicAllocator.allocate(indexDataSize, sce::Gnm::kAlignmentOfBufferInBytes));
	vertexBuffer = static_cast<MeshVertex*>	(garlicAllocator.allocate(vertexDataSize, sce::Gnm::kAlignmentOfBufferInBytes));

	sce::Gnm::registerResource(nullptr, ownerHandle, indexBuffer, indexDataSize, "IndexData", sce::Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	sce::Gnm::registerResource(nullptr, ownerHandle, vertexBuffer, vertexDataSize, "VertexData", sce::Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	for (unsigned int i = 0; i < m_NumVertices; ++i)
	{
		memcpy(&vertexBuffer[i].position, &m_Vertices[i], sizeof(float) * 3);
		memcpy(&vertexBuffer[i].textureCoord, &m_TextureCoords[i], sizeof(float) * 2);

		if (m_Normals)
			memcpy(&vertexBuffer[i].normal, &m_Normals[i], sizeof(float) * 3);
		
		if (m_Tangents)
			memcpy(&vertexBuffer[i].tangent, &m_Tangents[i], sizeof(float) * 3);
	}

	for (unsigned int i = 0; i < m_NumIndices; ++i) 
	{ //Our index buffer might not have the same data size as the source indices?
		indexBuffer[i] = m_Indices[i];
	}

	attributeCount = 4;	
	attributeBuffers = new sce::Gnm::Buffer[attributeCount];

	InitAttributeBuffer(attributeBuffers[0], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], sce::Gnm::kDataFormatR32G32Float, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], sce::Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].tangent));

	//Clean();

	for (auto child : m_Children)
		child->BufferData();
}

void PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer& buffer, sce::Gnm::DataFormat format, void* offset)
{
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), m_NumVertices);
	buffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
}

void PS4Mesh::Draw(Material* material)
{
	//reserved textures
	for (unsigned int i = 0; i < ReservedMeshTextures.size; ++i)
	{
		shaderResourceLocation slot;
		slot.id = i;
		if (m_Textures[i])
			m_Textures[i]->Load(slot);
	}
	//reserved colours
	for (unsigned int i = 0; i < ReservedMeshColours.size; ++i)
		Renderer::GetInstance()->UpdateUniform(material->GetShader()->GetReservedMeshColourUniformLocation(i), GetColour(i));
	
	//reserved float
	Renderer::GetInstance()->UpdateUniform(material->GetShader()->GetResourceByName("specExponent"), m_SpecExponent); 
	material->Setup();

	SubmitDraw(*Renderer::GetInstance()->GetGFXContext(), sce::Gnm::ShaderStage::kShaderStageVs);

	for (auto child : m_Children)
		child->Draw(material);
}

void PS4Mesh::SubmitDraw(sce::Gnmx::GnmxGfxContext& cmdList, sce::Gnm::ShaderStage stage)
{
	cmdList.setVertexBuffers(stage, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);	
	cmdList.drawIndex(m_NumIndices, indexBuffer);
}
#endif