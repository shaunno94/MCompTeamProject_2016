/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef ORBIS
#include "Dependencies/glew-1.13.0/include/GL/glew.h"
#include "Dependencies/glew-1.13.0/include/GL/wglew.h"
#include <string>
#include "constants.h"
#include "Shader.h"

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TCS 3
#define SHADER_TES 4
#define SHADER_MAX_STAGE_COUNT 5

using namespace std;

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class OGLShader : public BaseShader
{
public:
	OGLShader(string vertex, string fragment, string geometry = "", string tcsFile = "", string tesFile = "");
	~OGLShader(void);

	inline GLuint  GetProgram() const
	{
		return program;
	}
	inline bool IsOperational() const override
	{
		return operational == GL_TRUE;
	}

	inline GLint GetModelMatrixUniformLocation() const
	{
		return m_ModelMatrixLocation;
	}
	inline GLint GetReservedMeshTextureUniformLocation(size_t index) const override
	{
		return m_ReservedMeshTextureLocations[index];
	}
	inline GLint GetReservedMeshColourUniformLocation(size_t index) const
	{
		return m_ReservedMeshColourLocations[index];
	}

	int GetResourceByName(const std::string& name) const override
	{
		return glGetUniformLocation(program, name.c_str());
	}

protected:
	GLint operational;
	bool	LoadShaderFile(string from, string& into);
	GLuint	GenerateShader(string from, GLenum type);
	void	SetDefaultAttributes();

	GLuint objects[SHADER_MAX_STAGE_COUNT];
	GLuint program;

	GLint m_ModelMatrixLocation;
	GLint m_ReservedMeshTextureLocations[ReservedMeshTextures.size];
	GLint m_ReservedMeshColourLocations[ReservedMeshColours.size];
};
#endif
