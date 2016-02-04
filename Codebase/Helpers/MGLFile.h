#pragma once
#include "stdafx.h"

#include "MGLUtil.h"

/*
	Loads in OBJ and MGL files
*/

class MGLMesh;

class MGLFile {
public:
	// Loads file and returns correct MGLMesh, buffered or unbuffered
	virtual MGLMesh* Load(std::string fileName, GLboolean bufferData = GL_TRUE) = 0;
	// Saves a given mesh to .mgl format
	void SaveMeshToMGL(MGLMesh* mesh, std::string fileName, GLboolean saveColours);
protected:
	// Singleton protection
	MGLFile(std::string ext) : m_fileEXT(ext) {}
	virtual ~MGLFile(){}

	/****** Methods ******/

	// Gets a istringstream of a full file
	std::stringstream* LoadFileToSS(std::string fileName);
	// Loads binary file and returns a vector of required type
	template < typename T >
	std::vector<T>* LoadFileToVec(std::string fileName);

	/****** Data ******/

	std::string m_fileEXT;
};

/* OBJ */

class MGLFileOBJ : public MGLFile, public MGLSingleton<MGLFileOBJ> {
	friend class MGLSingleton < MGLFileOBJ >;
public:
	// Load OBJ, does NOT need file extension!
	virtual MGLMesh* Load(std::string fileName, GLboolean bufferData = GL_TRUE);
protected:
	MGLFileOBJ() : MGLFile(".obj") {}
	virtual ~MGLFileOBJ(){}

	GLboolean HandleOBJFace(std::string line, MGLObjFileData* obj);
	GLuint AddOBJVertToList(MGLObjFileData* obj, MGLObjVertData& vert);
	MGLMesh* CreateMesh(MGLObjFileData* obj);

	// Handle OBJ face with no normals
	void AddFaceMissing(MGLObjFileData* obj, MGLvecu& indices, const GLboolean isPolygon, const GLboolean skipTex);
	// Handle OBJ face
	void AddFace(MGLObjFileData* obj, MGLvecu& indices, const GLboolean isPolygon);
};

/* MGL */

class MGLFileMGL : public MGLFile, public MGLSingleton<MGLFileMGL> {
	friend class MGLSingleton < MGLFileMGL >;
public:
	// Load MGL, does NOT need file extension!
	virtual MGLMesh* Load(std::string fileName, GLboolean bufferData = GL_TRUE);
protected:
	MGLFileMGL() : MGLFile(".mgl") {}
	virtual ~MGLFileMGL(){}

	/****** Methods ******/

	// Determines correct file size based on inputs
	std::size_t DetermineFileSize(const GLuint numVertices, const GLuint numIndices, const GLint colourVal);
	// Creates a MGLMesh from a loaded buffer
	MGLMesh* LoadMesh(const MGLvecf* buffer);
};

/* .mgl file structure */
/* Based on data from MGLMesh */
/* Saved in binary, size of GLfloat (currently) */
/*
*FILE_START*
file verison
type
numVertices
numIndices
*COLOUR USE*
-1 = one colour for all vertices
0 = no colours
>0 = number of colours
*VERTICES*
vec1.x
vec1.y
vec1.z
vec2.x
vec2.y
...
*TEXCOORDS*
vec1.x
vec1.y
vec2.x
vec2.y
vec3.x
...
*NORMALS*
vec1.x
vec1.y
vec1.z
vec2.x
vec2.y
...
*COLOURS* <-- optional, based on *COLOUR USE*
vec1.x
vec1.y
vec1.z
vec1.w
vec2.x
...
*INDICES*
ind1
in2
ind3
ind4
...
*FILE_END*
*/