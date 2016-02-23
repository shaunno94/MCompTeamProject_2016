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
#ifndef ORBIS
#pragma once
#include "Mesh.h"
#include "OGLRenderer.h"

class Material;

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class OGLMesh : public Mesh
{
public:

	OGLMesh(void);
	OGLMesh(size_t numVertices, Vec3Graphics* vertices, Vec2Graphics* texCoords, Vec3Graphics* normals, Vec3Graphics* tangents, size_t numIndices, size_t* indices);
	virtual ~OGLMesh(void);

	void Draw(Material* material) override;

	void DrawDebugNormals(float length = 5.0f) override;
	void DrawDebugTangents(float length = 5.0f) override;	
	
	//Buffers all VBO data into graphics memory. Required before drawing!
	void BufferData() override;
	void SetPrimitiveType(pType type) override;
protected:
	//VAO for this mesh
	GLuint	arrayObject;
	//VBOs for this mesh
	GLuint	bufferObject[MAX_BUFFER];
	//Primitive type for this mesh (GL_TRIANGLES...etc)
	GLuint	type;
};
#endif