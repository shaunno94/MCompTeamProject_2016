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

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	,
	COLOUR_BUFFER	, 
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	, 
	TANGENT_BUFFER	,
	INDEX_BUFFER	,
	MAX_BUFFER
};






class Mesh	{
public:
	friend class MD5Mesh;
	Mesh(void);
	virtual ~Mesh(void);

	void Draw();

	//Generates a single triangle, with RGB colours
	static Mesh*	GenerateTriangle();
	

	//Generates a single white quad, going from -1 to 1 on the x and z axis.
	static Mesh*	GenerateQuad();
	static Mesh*	GenerateQuadAlt();
	//Generates a coloured quad, going from -1 to 1 on the x and z axis, with adjustable texture coords.
	static Mesh*	GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour); //NX 01/11/2012

	static Mesh*	GenerateIcosphere(unsigned int tessalationLevel);

	//Sets the Mesh's diffuse map. Takes an OpenGL texture 'name'
	void	SetTexture(GLuint tex)	{texture = tex;}
	//Gets the Mesh's diffuse map. Returns an OpenGL texture 'name'
	GLuint  GetTexture()			{return texture;}

	void	SetColour(Vec4Graphics* colour) {colours = colour;} //NX 24/10/2012

	//Sets the Mesh's bump map. Takes an OpenGL texture 'name'
	void	SetBumpMap(GLuint tex)	{bumpTexture = tex;}
	//Gets the Mesh's bump map. Returns an OpenGL texture 'name'
	GLuint  GetBumpMap()			{return bumpTexture;}

	//Extra stuff!!!! Aren't I nice?
	void	DrawDebugNormals(float length = 5.0f);
	void	DrawDebugTangents(float length = 5.0f);

	bool	TransformsTexCoords() { return transformCoords;}

	//Generates normals for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateNormals();

	//Generates tangents for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateTangents();

protected:
	//Buffers all VBO data into graphics memory. Required before drawing!
	void	BufferData();

	//Helper function for GenerateTangents
	Vec3Graphics GenerateTangent(const Vec3Graphics &a,const Vec3Graphics &b,const Vec3Graphics &c,const Vec2Graphics &ta,const Vec2Graphics &tb,const Vec2Graphics &tc);

	//VAO for this mesh
	GLuint	arrayObject;
	//VBOs for this mesh
	GLuint	bufferObject[MAX_BUFFER];
	//Number of vertices for this mesh
	GLuint	numVertices;
	//Primitive type for this mesh (GL_TRIANGLES...etc)
	GLuint	type;
	//OpenGL texture name for the diffuse map
	GLuint	texture;

	//Stuff introduced later on in the tutorials!!

	//Number of indices for this mesh
	GLuint			numIndices;

	//OpenGL texture name for the bump map
	GLuint			bumpTexture;

	//You might wonder why we keep pointers to vertex data once
	//it's sent off to graphics memory. For basic meshes, there's no
	//real need to keep it, so feel free to delete it. For MD5Meshes,
	//we need access to the vertex data for skinning per frame...

	//Pointer to vertex position attribute data (badly named...?)
	Vec3Graphics*		vertices;
	//Pointer to vertex colour attribute data
	Vec4Graphics*		colours;
	//Pointer to vertex texture coordinate attribute data
	Vec2Graphics*		textureCoords;
	//Pointer to vertex normals attribute data
	Vec3Graphics*		normals;
	//Pointer to vertex tangents attribute data
	Vec3Graphics*		tangents;
	//Pointer to vertex indices attribute data
	unsigned int*	indices;


	bool			transformCoords;
};

