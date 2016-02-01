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


#include "OGLRenderer.h"

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TCS 3
#define SHADER_TES 4
#define SHADER_MAX_STAGE_COUNT 5

using namespace std;
class Shader	{
public:
	Shader(string vertex, string fragment , string geometry = "", string tcsFile = "", string tesFile = "");
	~Shader(void);

	GLuint  GetProgram() { return program;}
	inline bool IsOperational() const { return operational == GL_TRUE; }

protected:
	GLint operational;
	bool	LoadShaderFile(string from, string &into);
	GLuint	GenerateShader(string from, GLenum type);
	void	SetDefaultAttributes();

	GLuint objects[SHADER_MAX_STAGE_COUNT];
	GLuint program;
};

