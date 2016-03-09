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
#include <vector>
#include "Math/nclglMath.h"
#include "constants.h"
#include "Texture.h"
#include "Font.h"
#include <cstdint>

#ifdef _DEBUG
#define MESH_TRACK_STATS
#endif

class Material;

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

enum pType
{
	TRIANGLE, TRIANGLE_STRIP, TRIANGLE_LIST
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
	Mesh(uint32_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, uint32_t numIndices, uint32_t* indices);
	virtual ~Mesh(void);

	virtual void Draw(Material* material) = 0;

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
	static Mesh*	GenerateQuad(Vec2Graphics texCoords = Vec2Graphics(1.0f, 1.0f));

	// Draw quad between 4 vertices (Created for Stadium walls)
	static Mesh*	GenerateQuad(Vec3Graphics* vertices, Vec2Graphics texCoords = Vec2Graphics(1.0f, 1.0f));
	static Mesh*	GenerateTextQuad(const std::string& text, Font* font);

	static Mesh*	GenerateQuadAlt();
	//Generates a coloured quad, going from -1 to 1 on the x and z axis, with adjustable texture coords.
	static Mesh*	GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour); //NX 01/11/2012


	//Gets the Mesh's diffuse map. Returns an OpenGL texture 'name'
	inline Texture*  GetTexture(uint32_t index) const
	{
		return m_Textures[index];
	}
	inline const Vec3Graphics& GetColour(uint32_t index) const
	{
		return m_Colours[index];
	}
	inline float GetSpecExponent() const
	{
		return m_SpecExponent;
	}
	inline uint32_t GetNumVertices()
	{
		return m_NumVertices;
	}
	inline uint32_t GetNumIndices()
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
	inline uint32_t* GetIndices()
	{
		return m_Indices;
	}

	//Extra stuff!!!! Aren't I nice?
	virtual void DrawDebugNormals(float length) = 0;
	virtual void DrawDebugTangents(float length) = 0;

	//Generates normals for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateNormals();

	//Generates tangents for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateTangents();

	void	SetMtlData(const MeshMtlData& data);
	void	SetTexture(Texture* tex, uint32_t index);
	//Buffers all VBO data into graphics memory. Required before drawing!
	virtual void BufferData() = 0;
	virtual void SetPrimitiveType(pType type) = 0;

	static uint64_t GetMeshMemUsage() { return MeshMemoryUsage; }

protected:
	//Helper function for GenerateTangents
	Vec3Graphics GenerateTangent(const Vec3Graphics& a,const Vec3Graphics& b,const Vec3Graphics& c,const Vec2Graphics& ta,const Vec2Graphics& tb,const Vec2Graphics& tc);

	std::vector<Mesh*> m_Children;

	void Clean();

	//Number of vertices for this mesh
	uint32_t m_NumVertices;
	//Number of indices for this mesh
	uint32_t m_NumIndices;

	Texture* m_Textures[ReservedMeshTextures.size];
	Vec3Graphics m_Colours[ReservedMeshColours.size];
	float m_SpecExponent;

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
	uint32_t*	m_Indices;

	static void CalcMeshUsage(Mesh* m);
	static uint64_t MeshMemoryUsage;
};
