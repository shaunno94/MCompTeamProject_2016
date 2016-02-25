#include "Mesh.h"
#ifndef ORBIS
#include "OGLMesh.h"
#else
#include "PS4Mesh.h"
#endif

Mesh::Mesh(void)
{
	m_Textures[ReservedMeshTextures.size];
	m_Colours[ReservedMeshColours.size];

	for (auto& t : m_Textures)
	{
		t = nullptr;
	}
	for (auto& c: m_Colours)
	{
		c = Vec3Graphics::ZEROS;
	}

	m_SpecExponent = 0.0f;

	m_Colours[ReservedMeshColours.AMBIENT.index] = Vec3Graphics(0.2f, 0.2f, 0.2);
	m_Colours[ReservedMeshColours.DIFFUSE.index] = Vec3Graphics::ONES;
	m_Colours[ReservedMeshColours.SPECULAR.index] = Vec3Graphics::ONES;

	m_NumVertices = 0;
	m_NumIndices = 0;
	m_Vertices = nullptr;
	m_TextureCoords = nullptr;
	m_Normals = nullptr;
	m_Tangents = nullptr;
	m_Indices = nullptr;
	m_Children.clear();
}

Mesh::Mesh(uint32_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, uint32_t numIndices, uint32_t* indices)
{
	m_Textures[ReservedMeshTextures.size];
	m_Colours[ReservedMeshColours.size];

	for (auto& t : m_Textures)
	{
		t = nullptr;
	}
	for (auto& c : m_Colours)
	{
		c = Vec3Graphics::ZEROS;
	}

	m_SpecExponent = 0.0f;

	m_NumVertices = 0;

	m_Colours[ReservedMeshColours.AMBIENT.index] = Vec3Graphics(0.2f, 0.2f, 0.2f);
	m_Colours[ReservedMeshColours.DIFFUSE.index] = Vec3Graphics::ONES;
	m_Colours[ReservedMeshColours.SPECULAR.index] = Vec3Graphics::ONES;

	m_NumVertices = numVertices;
	m_NumIndices = numIndices;
	m_Vertices = vertices;
	m_TextureCoords = texCoords;
	m_Normals = normals;
	m_Tangents = tangents;
	m_Indices = indices;
	m_Children.clear();
}

Mesh::~Mesh(void)
{
	for (uint32_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
			m_Textures[i]->Clear();
	}
	Clean();
}

void Mesh::Clean()
{
	if (m_Vertices)
		delete[] m_Vertices;

	if (m_Indices)
		delete[] m_Indices;

	if (m_TextureCoords)
		delete[] m_TextureCoords;

	if (m_Tangents)
		delete[] m_Tangents;

	if (m_Normals)
		delete[] m_Normals;
}


Mesh* Mesh::GenerateQuad(Vec2Graphics texCoords)
{
#ifndef ORBIS
	Mesh* m = new OGLMesh();
#else
	Mesh* m = new PS4Mesh();
#endif
	
	m->m_NumVertices = 4;
	m->m_NumIndices = 4;
	m->SetPrimitiveType(TRIANGLE_STRIP);

	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	m->m_Indices = new uint32_t[m->m_NumIndices];
	m->m_TextureCoords = new Vec2Graphics[m->m_NumVertices];
	m->m_Normals = new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents = new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = Vec3Graphics(-1.0f, -1.0f, 0.0f);
	m->m_Vertices[1] = Vec3Graphics(-1.0f, 1.0f, 0.0f);
	m->m_Vertices[2] = Vec3Graphics(1.0f, -1.0f, 0.0f);
	m->m_Vertices[3] = Vec3Graphics(1.0f, 1.0f, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(0.0f, texCoords.y);
	m->m_TextureCoords[1] = Vec2Graphics(0.0f, 0.0f);
	m->m_TextureCoords[2] = Vec2Graphics(texCoords.x, texCoords.y);
	m->m_TextureCoords[3] = Vec2Graphics(texCoords.x, 0.0f);

	for (int i = 0; i < m->m_NumIndices; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f, -1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f, 0.0f);
		m->m_Indices[i] = i;
	}
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuad(Vec3Graphics* vertices, Vec2Graphics texCoords /* = Vec2Graphics(1.0f, 1.0f) */)
{
#ifndef ORBIS
	Mesh* m = new OGLMesh();
#else
	Mesh* m = new PS4Mesh();
#endif

	m->m_NumVertices = 4;
	m->m_NumIndices = 4;
	m->SetPrimitiveType(TRIANGLE_STRIP);

	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	m->m_Indices = new size_t[m->m_NumIndices];
	m->m_TextureCoords = new Vec2Graphics[m->m_NumVertices];
	m->m_Normals = new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents = new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = vertices[0];
	m->m_Vertices[1] = vertices[1];
	m->m_Vertices[2] = vertices[3];
	m->m_Vertices[3] = vertices[2];

	m->m_TextureCoords[0] = Vec2Graphics(0.0f, texCoords.y);
	m->m_TextureCoords[1] = Vec2Graphics(0.0f, 0.0f);
	m->m_TextureCoords[2] = Vec2Graphics(texCoords.x, texCoords.y);
	m->m_TextureCoords[3] = Vec2Graphics(texCoords.x, 0.0f);

	Vec3Graphics normal = (vertices[1] - vertices[0]).Cross(vertices[2] - vertices[0]);

	for (int i = 0; i < m->m_NumIndices; ++i)
	{
		m->m_Normals[i] = normal;
		m->m_Tangents[i] = normal.Cross(normal);
		m->m_Indices[i] = i;
	}
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuadAlt()
{
#ifndef ORBIS
	Mesh* m = new OGLMesh();
#else
	Mesh* m = new PS4Mesh();
#endif

	m->m_NumVertices = 4;
	m->m_NumIndices = 6;
	m->SetPrimitiveType(TRIANGLE_STRIP);

	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	m->m_Indices = new uint32_t[m->m_NumIndices];
	m->m_TextureCoords = new Vec2Graphics[m->m_NumVertices];
	m->m_Normals = new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents = new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->m_Vertices[1] = Vec3Graphics(0.0f, 1.0f, 0.0f);
	m->m_Vertices[2] = Vec3Graphics(1.0f, 0.0f, 0.0f);
	m->m_Vertices[3] = Vec3Graphics(1.0f, 1.0f, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(0.0f, 0.0f);
	m->m_TextureCoords[1] = Vec2Graphics(0.0f, 1.0f);
	m->m_TextureCoords[2] = Vec2Graphics(1.0f, 0.0f);
	m->m_TextureCoords[3] = Vec2Graphics(1.0f, 1.0f);

	for (int i = 0; i < m->m_NumIndices; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f, -1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f, 0.0f);
		m->m_Indices[i] = i;
	}
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour)
{
#ifndef ORBIS
	Mesh* m = new OGLMesh();
#else
	Mesh* m = new PS4Mesh();
#endif

	m->m_NumVertices = 4;
	m->m_NumIndices = 6;
	m->SetPrimitiveType(TRIANGLE_STRIP);

	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	m->m_Indices = new uint32_t[m->m_NumIndices];
	m->m_TextureCoords = new Vec2Graphics[m->m_NumVertices];
	m->m_Normals = new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents = new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->m_Vertices[2] = Vec3Graphics(0.0f, scale.y, 0.0f);
	m->m_Vertices[1] = Vec3Graphics(scale.x, 0.0f, 0.0f);
	m->m_Vertices[3] = Vec3Graphics(scale.x, scale.y, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(texCoord.x, texCoord.x);
	m->m_TextureCoords[2] = Vec2Graphics(texCoord.x, texCoord.y);
	m->m_TextureCoords[1] = Vec2Graphics(texCoord.y, texCoord.x);
	m->m_TextureCoords[3] = Vec2Graphics(texCoord.y, texCoord.y);

	for (int i = 0; i  < m->m_NumIndices; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f, -1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f, 0.0f);
		m->m_Indices[i] = i;
	}
	m->BufferData();
	return m;
}

void Mesh::GenerateNormals()
{
	if (!m_Normals)
		m_Normals = new Vec3Graphics[m_NumVertices];
	else
		return;

	for (unsigned int i = 0; i < m_NumVertices; ++i)
		m_Normals[i] = Vec3Graphics();

	if (m_Indices)
	{
		 for (unsigned int i = 0; i < m_NumIndices; i += 3)
		{
			int a = m_Indices[i];
			int b = m_Indices[i + 1];
			int c = m_Indices[i + 2];

			Vec3Graphics normal = (m_Vertices[b] - m_Vertices[a]).Cross(m_Vertices[c] - m_Vertices[a]);

			m_Normals[a] += normal;
			m_Normals[b] += normal;
			m_Normals[c] += normal;
		}
	}
	else
	{
		//It's just a list of triangles, so generate face normals
		for (unsigned int i = 0; i < m_NumVertices; i += 3)
		{
			Vec3Graphics& a = m_Vertices[i];
			Vec3Graphics& b = m_Vertices[i + 1];
			Vec3Graphics& c = m_Vertices[i + 2];

			Vec3Graphics normal = (a - b).Cross(a - c);

			m_Normals[i] = normal;
			m_Normals[i + 1] = normal;
			m_Normals[i + 2] = normal;
		}
	}

	for (unsigned int i = 0; i < m_NumVertices; ++i)
		m_Normals[i].Normalize();
}

void Mesh::GenerateTangents()
{
	//Extra! stops rare occurrence of this function being called
	//on a mesh without tex coords, which would break quite badly!
	if (!m_TextureCoords)
		return;

	if (m_Tangents)
		return;

	if (!m_Tangents)
		m_Tangents = new Vec3Graphics[m_NumVertices];
	for (unsigned int i = 0; i < m_NumVertices; ++i)
		m_Tangents[i] = Vec3Graphics();

	if (m_Indices)
	{
		for (unsigned int i = 0; i < m_NumIndices; i += 3)
		{
			int a = m_Indices[i];
			int b = m_Indices[i + 1];
			int c = m_Indices[i + 2];

			Vec3Graphics tangent = GenerateTangent(m_Vertices[a], m_Vertices[b], m_Vertices[c], m_TextureCoords[a], m_TextureCoords[b], m_TextureCoords[c]);

			m_Tangents[a] += tangent;
			m_Tangents[b] += tangent;
			m_Tangents[c] += tangent;
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_NumVertices; i += 3)
		{
			Vec3Graphics tangent = GenerateTangent(m_Vertices[i], m_Vertices[i + 1], m_Vertices[i + 2], m_TextureCoords[i], m_TextureCoords[i + 1], m_TextureCoords[i + 2]);

			m_Tangents[i] += tangent;
			m_Tangents[i + 1] += tangent;
			m_Tangents[i + 2] += tangent;
		}
	}
	for (unsigned int i = 0; i < m_NumVertices; ++i)
		m_Tangents[i].Normalize();
}

Vec3Graphics Mesh::GenerateTangent(const Vec3Graphics& a, const Vec3Graphics& b, const Vec3Graphics& c, const Vec2Graphics& ta, const Vec2Graphics& tb, const Vec2Graphics& tc)
{
	Vec2Graphics coord1 = tb - ta;
	Vec2Graphics coord2 = tc - ta;

	Vec3Graphics vertex1 = b - a;
	Vec3Graphics vertex2 = c - a;

	Vec3Graphics axis = Vec3Graphics(vertex1*coord2.y - vertex2*coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}

void Mesh::SetTexture(Texture* tex, uint32_t index)
{
	if (m_Textures[index])
		m_Textures[index]->Clear();
	m_Textures[index] = tex;
	if (m_Textures[index])
		m_Textures[index]->ReserveCopy();
}

void Mesh::SetMtlData(const MeshMtlData& data)
{
	for (uint32_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
			m_Textures[i]->Clear();
		m_Textures[i] = data.textureMaps[i];
		if (m_Textures[i])
			m_Textures[i]->ReserveCopy();
	}
	for (uint32_t i = 0; i < ReservedMeshColours.size; ++i)
	{
		m_Colours[i] = data.colours[i];
	}
	m_SpecExponent = data.specExponent;
}