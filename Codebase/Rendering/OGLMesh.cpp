#ifndef ORBIS
#include "OGLMesh.h"
#include <iostream>

#include "ModelLoader.h"
#include "Material.h"
#include "Renderer.h"
#include "OGLShader.h"

OGLMesh::OGLMesh(void)
{
	for (int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;
	type = GL_TRIANGLES;
}

OGLMesh::OGLMesh(uint32_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, uint32_t numIndices, uint32_t* indices) 
: Mesh(numVertices, vertices, texCoords, normals, tangents, numIndices, indices)
{
	for (unsigned int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;

	type = GL_TRIANGLES;
}

OGLMesh::~OGLMesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);			//Delete VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete VBOs
}


void OGLMesh::Draw(Material* material)
{
	//reset reserved mesh texture uniforms to use the right texture unit
	for (int i = 0; i < ReservedMeshTextures.size; ++i)
		Renderer::GetInstance()->UpdateUniform(material->GetShader()->GetReservedMeshTextureUniformLocation(i), i);

	//reserved textures
	for (unsigned int i = 0; i < ReservedMeshTextures.size; ++i)
	{
		if (m_Textures[i])
		{
			shaderResourceLocation location;
			location.id = i;
			m_Textures[i]->Load(location);
		}
	}
	//reserved colours
	for (unsigned int i = 0; i < ReservedMeshColours.size; ++i)
		Renderer::GetInstance()->UpdateUniform(static_cast<OGLShader*>(material->GetShader())->GetReservedMeshColourUniformLocation(i), GetColour(i));

	//reserved float
	shaderResourceLocation location = { glGetUniformLocation(static_cast<OGLShader*>(material->GetShader())->GetProgram(), "specExponent") };
	Renderer::GetInstance()->UpdateUniform(location, GetSpecExponent());

	material->Setup();

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, m_NumIndices, GL_UNSIGNED_INT, 0);
	}	
	else
	{
		glDrawArrays(type, 0, m_NumVertices);	//Draw the triangle!
	}
	glBindVertexArray(0);

	for (auto child : m_Children)
		child->Draw(material);
}

void OGLMesh::BufferData()
{
	//moved here from constructor to not require OpenGL instance when using Mesh for format conversion
	glGenVertexArrays(1, &arrayObject);

	GenerateNormals();
	GenerateTangents();

	glBindVertexArray(arrayObject);

	//Buffer vertex data
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_NumVertices * sizeof(Vec3Graphics), m_Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	//Buffer texture data
	if (m_TextureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices * sizeof(Vec2Graphics), m_TextureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2Graphics), 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	//Buffer normal data
	if (m_Normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices * sizeof(Vec3Graphics), m_Normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	//Buffer tangent data
	if (m_Tangents)
	{
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices * sizeof(Vec3Graphics), m_Tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3Graphics), 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	//buffer index data
	if (m_Indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NumIndices * sizeof(unsigned int), m_Indices, GL_STATIC_DRAW);
	}

	for (auto& child : m_Children)
		child->BufferData();

	glBindVertexArray(0);
}

void OGLMesh::DrawDebugNormals(float length)
{
	if (m_NumVertices > 0)
	{
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[m_NumVertices * 2];
		Vec3Graphics* tempC = new Vec3Graphics[m_NumVertices * 2];

		for (GLuint i = 0; i < m_NumVertices; ++i)
		{
			tempC[(i * 2)] = Vec3Graphics(1, 0, 0);
			tempC[(i * 2) + 1] = Vec3Graphics(1, 1, 1);

			tempV[(i * 2)] = m_Vertices[i];
			tempV[(i * 2) + 1] = m_Vertices[i] + (m_Normals[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)* 2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)* 2, tempC, GL_STREAM_DRAW);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS, 0, m_NumVertices * 2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES, 0, m_NumVertices * 2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	for(unsigned int i = 0; i < m_Children.size(); ++i) {
		m_Children[i]->DrawDebugNormals(length);
	}
}

void OGLMesh::DrawDebugTangents(float length)
{
	if (m_NumVertices > 0)
	{
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[m_NumVertices * 2];
		Vec3Graphics* tempC = new Vec3Graphics[m_NumVertices * 2];

		for (GLuint i = 0; i < m_NumVertices; ++i)
		{
			tempC[(i * 2)] = Vec3Graphics(1, 1, 1);
			tempC[(i * 2) + 1] = Vec3Graphics(0, 1, 0);

			tempV[(i * 2)] = m_Vertices[i];
			tempV[(i * 2) + 1] = m_Vertices[i] + (m_Tangents[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)* 2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, m_NumVertices*sizeof(Vec3Graphics)* 2, tempC, GL_STREAM_DRAW);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS, 0, m_NumVertices * 2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES, 0, m_NumVertices * 2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	for(unsigned int i = 0; i < m_Children.size(); ++i) {
		m_Children[i]->DrawDebugTangents(length);
	}
}

void OGLMesh::SetPrimitiveType(pType type)
{
	switch (type)
	{
	case TRIANGLE:
	this->type = GL_TRIANGLES;
		break;
	case TRIANGLE_STRIP:
		this->type = GL_TRIANGLE_STRIP;
		break;
	}
}
#endif