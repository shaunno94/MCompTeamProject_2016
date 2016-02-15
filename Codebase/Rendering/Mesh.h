/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents.


-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include "Math/nclglMath.h"
#include "constants.h"
#include "Texture.h"

class Material;

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
/// @ingroup Rendering
/// <summary>
/// Enumerator, to determine which member of the bufferObject array holds which data
/// </summary>
enum MeshBuffer
{
	VERTEX_BUFFER	,
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	,
	TANGENT_BUFFER	,
	INDEX_BUFFER	,
	MAX_BUFFER
};

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
struct MeshMtlData
{
	Texture* textureMaps[ReservedMeshTextures.size];
	Vec3Graphics colours[ReservedMeshColours.size];
	float specExponent;
};

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class Mesh
{
public:
	friend class ModelLoader;

	Mesh(void);
	Mesh(size_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, size_t numIndices, size_t* indices);
	virtual ~Mesh(void);

	void Draw(Material* material);

	inline void AddChild(Mesh* m)
	{
		m_Children.push_back(m);
	}
	inline const std::vector<Mesh*>& GetChildren()
	{
		return m_Children;
	}

	//Generates a single triangle, with RGB colours
	static Mesh*	GenerateTriangle();
	//Generates a single white quad, going from -1 to 1 on the x and z axis.
	static Mesh*	GenerateQuad();
	static Mesh*	GenerateQuadAlt();
	//Generates a coloured quad, going from -1 to 1 on the x and z axis, with adjustable texture coords.
	static Mesh*	GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour); //NX 01/11/2012

	static Mesh*	GenerateIcosphere(unsigned int tessalationLevel);

	//Gets the Mesh's diffuse map. Returns an OpenGL texture 'name'
	inline Texture*  GetTexture(size_t index) const
	{
		return m_Textures[index];
	}
	inline const Vec3Graphics& GetColour(size_t index) const
	{
		return m_Colours[index];
	}
	inline float GetSpecExponent() const
	{
		return m_SpecExponent;
	}
	inline size_t GetNumVertices()
	{
		return m_NumVertices;
	}
	inline size_t GetNumIndices()
	{
		return m_NumIndices;
	}
	inline Vec3Graphics* GetVertices()
	{
		return m_Vertices;
	}
	inline Vec3Graphics* GetNormals()
	{
		return m_Normals;
	}
	inline Vec3Graphics* GetTangents()
	{
		return m_Tangents;
	}
	inline Vec2Graphics* GetTextureCoords()
	{
		return m_TextureCoords;
	}
	inline size_t* GetIndices()
	{
		return m_Indices;
	}

	//Extra stuff!!!! Aren't I nice?
	void	DrawDebugNormals(float length = 5.0f);
	void	DrawDebugTangents(float length = 5.0f);

	//Generates normals for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateNormals();

	//Generates tangents for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateTangents();

	void	SetMtlData(const MeshMtlData& data);
	void	SetTexture(Texture* tex, size_t index);

protected:
	//Buffers all VBO data into graphics memory. Required before drawing!
	void	BufferData();

	//Helper function for GenerateTangents
	Vec3Graphics GenerateTangent(const Vec3Graphics& a,const Vec3Graphics& b,const Vec3Graphics& c,const Vec2Graphics& ta,const Vec2Graphics& tb,const Vec2Graphics& tc);


	std::vector<Mesh*> m_Children;

	//VAO for this mesh
	GLuint	arrayObject;
	//VBOs for this mesh
	GLuint	bufferObject[MAX_BUFFER];
	//Number of vertices for this mesh
	GLuint	m_NumVertices;
	//Primitive type for this mesh (GL_TRIANGLES...etc)
	GLuint	type;

	Texture* m_Textures[ReservedMeshTextures.size];
	Vec3Graphics m_Colours[ReservedMeshColours.size];
	float m_SpecExponent;

	//Number of indices for this mesh
	GLuint			m_NumIndices;

	//You might wonder why we keep pointers to vertex data once
	//it's sent off to graphics memory. For basic meshes, there's no
	//real need to keep it, so feel free to delete it. For MD5Meshes,
	//we need access to the vertex data for skinning per frame...

	//Pointer to vertex position attribute data (badly named...?)
	Vec3Graphics*		m_Vertices;
	//Pointer to vertex texture coordinate attribute data
	Vec2Graphics*		m_TextureCoords;
	//Pointer to vertex normals attribute data
	Vec3Graphics*		m_Normals;
	//Pointer to vertex tangents attribute data
	Vec3Graphics*		m_Tangents;
	//Pointer to vertex indices attribute data
	size_t*	m_Indices;
};

