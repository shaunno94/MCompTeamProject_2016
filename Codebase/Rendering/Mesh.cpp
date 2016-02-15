#include "Mesh.h"
#include <map>
#include <cstdint>
#include <iostream>
#include <cmath>

#include "ModelLoader.h"
#include "Material.h"
#include "Renderer.h"

Mesh::Mesh(void)
{
	for(int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;

	m_Textures[ReservedMeshTextures.size] = {};
	m_Colours[ReservedMeshColours.size] = {};
	m_SpecExponent = 0.0f;
	type		 = GL_TRIANGLES;

	m_Colours[ReservedMeshColours.AMBIENT.index] = Vec3Graphics(0.2f, 0.2f, 0.2);
	m_Colours[ReservedMeshColours.DIFFUSE.index] = Vec3Graphics::ONES;
	m_Colours[ReservedMeshColours.SPECULAR.index] = Vec3Graphics::ONES;

	m_NumVertices = 0;
	m_NumIndices    = 0;
	m_Vertices	  = nullptr;
	m_TextureCoords = nullptr;
	m_Normals		  = nullptr;
	m_Tangents	  = nullptr;
	m_Indices		  = nullptr;
}

Mesh::Mesh(size_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, size_t numIndices, size_t* indices)
{
	for (int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;

	m_Textures[ReservedMeshTextures.size] = {};
	m_Colours[ReservedMeshColours.size] = {};
	m_SpecExponent = 0.0f;
	type = GL_TRIANGLES;

	m_NumVertices = 0;
	type = GL_TRIANGLES;

	m_Colours[ReservedMeshColours.AMBIENT.index] = Vec3Graphics(0.2f, 0.2f, 0.2);
	m_Colours[ReservedMeshColours.DIFFUSE.index] = Vec3Graphics::ONES;
	m_Colours[ReservedMeshColours.SPECULAR.index] = Vec3Graphics::ONES;

	m_NumVertices = numVertices;
	m_NumIndices = numIndices;
	m_Vertices = vertices;
	m_TextureCoords = texCoords;
	m_Normals = normals;
	m_Tangents = tangents;
	m_Indices = indices;
}

Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs

	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
			m_Textures[i]->Clear();
	}

	//Later tutorial stuff
	delete[] m_Vertices;
	delete[] m_Indices;
	delete[] m_TextureCoords;
	delete[] m_Tangents;
	delete[] m_Normals;
}


void Mesh::Draw(Material* material)
{
	//reserved textures
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
			m_Textures[i]->Load(i);
	}
	//reserved colours
	for (size_t i = 0; i < ReservedMeshColours.size; ++i)
		Renderer::UpdateUniform(material->GetShader()->GetReservedMeshColourUniformLocation(i), GetColour(i));
	//reserved float
	Renderer::UpdateUniform(glGetUniformLocation(material->GetShader()->GetProgram(), "specExponent"), GetSpecExponent());

	material->Setup();

	glBindVertexArray(arrayObject);
	if(bufferObject[INDEX_BUFFER])
		glDrawElements(type, m_NumIndices, GL_UNSIGNED_INT, 0);
	else
	{
		glDrawArrays(type, 0, m_NumVertices);	//Draw the triangle!
	}
	glBindVertexArray(0);

	for (auto child : m_Children)
		child->Draw(material);
}

Mesh* Mesh::GenerateTriangle()
{
	Mesh* m = new Mesh();
	m->m_NumVertices = 3;

	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	m->m_Vertices[0] = Vec3Graphics(0.0f,	0.5f,	0.0f);
	m->m_Vertices[1] = Vec3Graphics(0.5f,  -0.5f,	0.0f);
	m->m_Vertices[2] = Vec3Graphics(-0.5f, -0.5f,	0.0f);

	m->m_TextureCoords = new Vec2Graphics[m->m_NumVertices];
	m->m_TextureCoords[0] = Vec2Graphics(0.5f,	0.0f);
	m->m_TextureCoords[1] = Vec2Graphics(1.0f,	1.0f);
	m->m_TextureCoords[2] = Vec2Graphics(0.0f,	1.0f);

	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();

	return m;
}

unsigned int IcosphereGetMidPoint(int p1, int p2, std::map<int64_t, unsigned int>& savedPoints, std::vector<Vec3Graphics>& vertices, unsigned int& currentIndex)
{
	// first check if we have it already
	bool firstIsSmaller = p1 < p2;
	int64_t smallerIndex = firstIsSmaller ? p1 : p2;
	int64_t greaterIndex = firstIsSmaller ? p2 : p1;
	int64_t key = (smallerIndex << 32) + greaterIndex;

	auto savedMatch = savedPoints.find(key);
	if(savedMatch != savedPoints.end())
		return savedMatch->second;

	//calculate new point
	Vec3Graphics middle = (vertices[p1] + vertices[p2]) * 0.5f;

	// add vertex makes sure point is on unit sphere
	vertices.push_back(middle.Normalize());

	// store it, return index
	savedPoints.insert(std::pair<uint64_t, unsigned int>(key, currentIndex));
	return currentIndex++;
}

Mesh* Mesh::GenerateIcosphere(unsigned int tessalationLevel)
{
	struct TriangleIndices
	{
	public:
		unsigned int v1;
		unsigned int v2;
		unsigned int v3;

		TriangleIndices(unsigned int v1, unsigned int v2, unsigned int v3) : v1(v1), v2(v2), v3(v3) {}
	};
	std::map<int64_t, unsigned int> tessalatedPoints;
	std::vector<Vec3Graphics> vertices;
	std::vector<unsigned int> indices;
	unsigned int currentIndex = 0;

	if(tessalationLevel == 0) return nullptr;

	// create 12 vertices of a icosahedron
	float offset = (1.0f + sqrt(5.0f)) * 0.5f;

	vertices.push_back(Vec3Graphics(-1,  offset,  0));
	vertices.push_back(Vec3Graphics(1,  offset,  0));
	vertices.push_back(Vec3Graphics(-1, -offset,  0));
	vertices.push_back(Vec3Graphics(1, -offset,  0));

	vertices.push_back(Vec3Graphics(0, -1,  offset));
	vertices.push_back(Vec3Graphics(0,  1,  offset));
	vertices.push_back(Vec3Graphics(0, -1, -offset));
	vertices.push_back(Vec3Graphics(0,  1, -offset));

	vertices.push_back(Vec3Graphics(offset,  0, -1));
	vertices.push_back(Vec3Graphics(offset,  0,  1));
	vertices.push_back(Vec3Graphics(-offset,  0, -1));
	vertices.push_back(Vec3Graphics(-offset,  0,  1));


	// create 20 triangles of the icosahedron
	auto faces = new std::vector<TriangleIndices>();

	// 5 faces around point 0
	faces->push_back(TriangleIndices(0, 11, 5));
	faces->push_back(TriangleIndices(0, 5, 1));
	faces->push_back(TriangleIndices(0, 1, 7));
	faces->push_back(TriangleIndices(0, 7, 10));
	faces->push_back(TriangleIndices(0, 10, 11));

	// 5 adjacent faces
	faces->push_back(TriangleIndices(1, 5, 9));
	faces->push_back(TriangleIndices(5, 11, 4));
	faces->push_back(TriangleIndices(11, 10, 2));
	faces->push_back(TriangleIndices(10, 7, 6));
	faces->push_back(TriangleIndices(7, 1, 8));

	// 5 faces around point 3
	faces->push_back(TriangleIndices(3, 9, 4));
	faces->push_back(TriangleIndices(3, 4, 2));
	faces->push_back(TriangleIndices(3, 2, 6));
	faces->push_back(TriangleIndices(3, 6, 8));
	faces->push_back(TriangleIndices(3, 8, 9));

	// 5 adjacent faces
	faces->push_back(TriangleIndices(4, 9, 5));
	faces->push_back(TriangleIndices(2, 4, 11));
	faces->push_back(TriangleIndices(6, 2, 10));
	faces->push_back(TriangleIndices(8, 6, 7));
	faces->push_back(TriangleIndices(9, 8, 1));


	// refine triangles
	for(int i = 0; i < tessalationLevel; i++)
	{
		auto tempFaces = new std::vector<TriangleIndices>();
		for(auto tri : *faces)
		{
			// replace triangle by 4 triangles
			int a = IcosphereGetMidPoint(tri.v1, tri.v2, tessalatedPoints, vertices, currentIndex);
			int b = IcosphereGetMidPoint(tri.v2, tri.v3, tessalatedPoints, vertices, currentIndex);
			int c = IcosphereGetMidPoint(tri.v3, tri.v1, tessalatedPoints, vertices, currentIndex);

			tempFaces->push_back(TriangleIndices(tri.v1, a, c));
			tempFaces->push_back(TriangleIndices(tri.v2, b, a));
			tempFaces->push_back(TriangleIndices(tri.v3, c, b));
			tempFaces->push_back(TriangleIndices(a, b, c));
		}
		delete faces;
		faces = tempFaces;
	}

	// done, now add triangles to mesh
	for(auto tri : *faces)
	{
		indices.push_back(tri.v1);
		indices.push_back(tri.v2);
		indices.push_back(tri.v3);
	}

	Mesh* m = new Mesh();
	m->m_NumVertices = vertices.size();
	m->m_Vertices = new Vec3Graphics[m->m_NumVertices];
	for(unsigned int i = 0; i < m->m_NumVertices; ++i)
	{
		m->m_Vertices[i] = vertices[i];
	}
	m->m_NumIndices = indices.size();
	m->m_Indices = new unsigned int[m->m_NumIndices];
	unsigned int counter = 0;
	for(auto face : *faces)
	{
		m->m_Indices[counter++] = face.v1;
		m->m_Indices[counter++] = face.v2;
		m->m_Indices[counter++] = face.v3;
	}

	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();

	return m;
}


Mesh* Mesh::GenerateQuad()
{
	Mesh* m = new Mesh();

	m->m_NumVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->m_Vertices			= new Vec3Graphics[m->m_NumVertices];
	m->m_TextureCoords	= new Vec2Graphics[m->m_NumVertices];
	m->m_Normals			= new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents			= new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = 	Vec3Graphics(-1.0f, -1.0f, 0.0f);
	m->m_Vertices[1] = 	Vec3Graphics(-1.0f,	1.0f, 0.0f);
	m->m_Vertices[2] = 	Vec3Graphics(1.0f, -1.0f, 0.0f);
	m->m_Vertices[3] = 	Vec3Graphics(1.0f,  1.0f, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(0.0f,	1.0f);
	m->m_TextureCoords[1] = Vec2Graphics(0.0f,	0.0f);
	m->m_TextureCoords[2] = Vec2Graphics(1.0f,	1.0f);
	m->m_TextureCoords[3] = Vec2Graphics(1.0f,	0.0f);

	for(int i = 0; i < 4; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f, -1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}


Mesh* Mesh::GenerateQuadAlt()
{
	Mesh* m = new Mesh();

	m->m_NumVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->m_Vertices			= new Vec3Graphics[m->m_NumVertices];
	m->m_TextureCoords	= new Vec2Graphics[m->m_NumVertices];
	m->m_Normals			= new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents			= new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = 	Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->m_Vertices[1] = 	Vec3Graphics(0.0f, 1.0f, 0.0f);
	m->m_Vertices[2] = 	Vec3Graphics(1.0f, 0.0f, 0.0f);
	m->m_Vertices[3] = 	Vec3Graphics(1.0f,  1.0f, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(0.0f,	0.0f);
	m->m_TextureCoords[1] = Vec2Graphics(0.0f,	1.0f);
	m->m_TextureCoords[2] = Vec2Graphics(1.0f,	0.0f);
	m->m_TextureCoords[3] = Vec2Graphics(1.0f,	1.0f);

	for(int i = 0; i < 4; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f,-1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}
//NX 01/11/2012
Mesh* Mesh::GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour)
{
	Mesh* m = new Mesh();

	m->m_NumVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->m_Vertices			= new Vec3Graphics[m->m_NumVertices];
	m->m_TextureCoords	= new Vec2Graphics[m->m_NumVertices];
	m->m_Normals			= new Vec3Graphics[m->m_NumVertices];
	m->m_Tangents			= new Vec3Graphics[m->m_NumVertices];

	m->m_Vertices[0] = 	Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->m_Vertices[2] = 	Vec3Graphics(0.0f, scale.y, 0.0f);
	m->m_Vertices[1] = 	Vec3Graphics(scale.x, 0.0f, 0.0f);
	m->m_Vertices[3] = 	Vec3Graphics(scale.x,  scale.y, 0.0f);

	m->m_TextureCoords[0] = Vec2Graphics(texCoord.x,	texCoord.x);
	m->m_TextureCoords[2] = Vec2Graphics(texCoord.x,	texCoord.y);
	m->m_TextureCoords[1] = Vec2Graphics(texCoord.y,	texCoord.x);
	m->m_TextureCoords[3] = Vec2Graphics(texCoord.y,	texCoord.y);

	for(int i = 0; i < 4; ++i)
	{
		m->m_Normals[i] = Vec3Graphics(0.0f, 0.0f, 1.0f);
		m->m_Tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}

void	Mesh::BufferData()
{
	//moved here from constructor to not require OpenGL instance when using Mesh for format conversion
	glGenVertexArrays(1, &arrayObject);

	//GenerateNormals();
	//GenerateTangents();

	glBindVertexArray(arrayObject);

	//Buffer vertex data
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics), m_Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	//Buffer texture data
	if(m_TextureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec2Graphics), m_TextureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2Graphics), 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	//Buffer normal data
	if(m_Normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics), m_Normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	//Buffer tangent data
	if(m_Tangents)
	{
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics), m_Tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	//buffer index data
	if(m_Indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NumIndices*sizeof(GLuint), m_Indices, GL_STATIC_DRAW);
	}

	for (auto child : m_Children)
		child->BufferData();

	glBindVertexArray(0);
}

/*
Stuff for later tutorials...
*/

void	Mesh::GenerateNormals()
{
	if(!m_Normals)
		m_Normals = new Vec3Graphics[m_NumVertices];
	for(GLuint i = 0; i < m_NumVertices; ++i)
		m_Normals[i] = Vec3Graphics();

	if(m_Indices)
	{
		GLuint i = 0;

		int test = 0;
		for(i = 0; i < m_NumIndices; i+=3)
		{
			int a = m_Indices[i];
			int b = m_Indices[i+1];
			int c = m_Indices[i+2];

			Vec3Graphics normal = (m_Vertices[b]-m_Vertices[a]).Cross(m_Vertices[c]-m_Vertices[a]);

			m_Normals[a] += normal;
			m_Normals[b] += normal;
			m_Normals[c] += normal;

			test+=3;
		}
		bool asdf = true;
	}
	else
	{
		//It's just a list of triangles, so generate face normals
		for(GLuint i = 0; i < m_NumVertices; i+=3)
		{
			Vec3Graphics& a = m_Vertices[i];
			Vec3Graphics& b = m_Vertices[i+1];
			Vec3Graphics& c = m_Vertices[i+2];

			Vec3Graphics normal = (b-a).Cross(c-a);

			m_Normals[i]	 = normal;
			m_Normals[i+1] = normal;
			m_Normals[i+2] = normal;
		}
	}

	for(GLuint i = 0; i < m_NumVertices; ++i)
		m_Normals[i].Normalize();
}

void Mesh::GenerateTangents()
{
	//Extra! stops rare occurrence of this function being called
	//on a mesh without tex coords, which would break quite badly!
	if(!m_TextureCoords)
		return;

	if(!m_Tangents)
		m_Tangents = new Vec3Graphics[m_NumVertices];
	for(GLuint i = 0; i < m_NumVertices; ++i)
		m_Tangents[i] = Vec3Graphics();

	if(m_Indices)
	{
		for(GLuint i = 0; i < m_NumIndices; i+=3)
		{
			int a = m_Indices[i];
			int b = m_Indices[i+1];
			int c = m_Indices[i+2];

			Vec3Graphics tangent = GenerateTangent(m_Vertices[a],m_Vertices[b],m_Vertices[c],m_TextureCoords[a],m_TextureCoords[b],m_TextureCoords[c]);

			m_Tangents[a] += tangent;
			m_Tangents[b] += tangent;
			m_Tangents[c] += tangent;
		}
	}
	else
	{
		for(GLuint i = 0; i < m_NumVertices; i+=3)
		{
			Vec3Graphics tangent = GenerateTangent(m_Vertices[i],m_Vertices[i+1],m_Vertices[i+2],m_TextureCoords[i],m_TextureCoords[i+1],m_TextureCoords[i+2]);

			m_Tangents[i]   += tangent;
			m_Tangents[i+1] += tangent;
			m_Tangents[i+2] += tangent;
		}
	}
	for(GLuint i = 0; i < m_NumVertices; ++i)
		m_Tangents[i].Normalize();
}

Vec3Graphics Mesh::GenerateTangent(const Vec3Graphics& a,const Vec3Graphics& b,const Vec3Graphics& c,const Vec2Graphics& ta,const Vec2Graphics& tb,const Vec2Graphics& tc)
{
	Vec2Graphics coord1  = tb-ta;
	Vec2Graphics coord2  = tc-ta;

	Vec3Graphics vertex1 = b-a;
	Vec3Graphics vertex2 = c-a;

	Vec3Graphics axis = Vec3Graphics(vertex1*coord2.y - vertex2*coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}

void Mesh::DrawDebugNormals(float length)
{
	if(m_NumVertices > 0)
	{
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[m_NumVertices*2];
		Vec3Graphics* tempC = new Vec3Graphics[m_NumVertices*2];

		for(GLuint i = 0; i < m_NumVertices; ++i)
		{
			tempC[(i*2)]  = Vec3Graphics(1,0,0);
			tempC[(i*2)+1] = Vec3Graphics(1,1,1);

			tempV[(i*2)]   = m_Vertices[i];
			tempV[(i*2)+1] = m_Vertices[i] + (m_Normals[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)*2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics) * 2, tempC, GL_STREAM_DRAW);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS,  0, m_NumVertices*2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES,  0, m_NumVertices*2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	//for(unsigned int i = 0; i < children.size(); ++i) {
	//	children.at(i)->DrawDebugNormals();
	//}
}

void Mesh::DrawDebugTangents(float length)
{
	if(m_NumVertices > 0)
	{
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[m_NumVertices*2];
		Vec3Graphics* tempC = new Vec3Graphics[m_NumVertices*2];

		for(GLuint i = 0; i < m_NumVertices; ++i)
		{
			tempC[(i*2)]  = Vec3Graphics(1,1,1);
			tempC[(i*2)+1] = Vec3Graphics(0,1,0);

			tempV[(i*2)]   = m_Vertices[i];
			tempV[(i*2)+1] = m_Vertices[i] + (m_Tangents[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)*2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)*2, tempC, GL_STREAM_DRAW);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS,  0, m_NumVertices*2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES,  0, m_NumVertices*2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	//for(unsigned int i = 0; i < children.size(); ++i) {
	//	children.at(i)->DrawDebugTangents();
	//}
}

void	Mesh::SetTexture(Texture* tex, size_t index)
{
	if (m_Textures[index])
		m_Textures[index]->Clear();
	m_Textures[index] = tex;
	if (m_Textures[index])
		m_Textures[index]->ReserveCopy();
}

void	Mesh::SetMtlData(const MeshMtlData& data)
{
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
			m_Textures[i]->Clear();
		m_Textures[i] = data.textureMaps[i];
		if (m_Textures[i])
			m_Textures[i]->ReserveCopy();
	}
	for (size_t i = 0; i < ReservedMeshColours.size; ++i)
	{
		m_Colours[i] = data.colours[i];
	}
	m_SpecExponent = data.specExponent;
}