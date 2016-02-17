#ifndef ORBIS
#pragma once
#include "Helpers/common.h"
#include <string>
#include <fstream>
#include <vector>

#include "Dependencies/glew-1.13.0/include/GL/glew.h"
#include "Dependencies/glew-1.13.0/include/GL/wglew.h"
#include "Math/nclglMath.h"
#include "Window.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew-1.13.0/lib/Release/Win32/glew32s.lib")

#include "OGLShader.h"		
#include "Mesh.h"		
#include <vector>

//#define OPENGL_DEBUGGING

/// @ingroup Rendering
/// <summary>
///
/// </summary>
static const float biasValues[16] =
{
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
/// @ingroup Rendering
/// <summary>
///
/// </summary>
static const Mat4Graphics biasMatrix(biasValues);

class OGLShader;

/// @ingroup Rendering
/// <summary>
/// OpenGL specific functionality for the Renderer.
/// </summary>
class OGLRenderer
{
	friend class Window;
public:
	OGLRenderer(std::string title, int sizeX, int sizeY, bool fullScreen);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene(float msec) = 0;
	virtual void	UpdateScene(float msec) = 0;
	void			SwapBuffers();

	bool			HasInitialised() const;


	OGLShader*			GetCurrentShader() const
	{
		return currentShader;
	}
	void			SetCurrentShader(OGLShader* s);

	static void UpdateUniform(GLint location, const Mat4Graphics& mat4);
	static void UpdateUniform(GLint location, const Mat3Graphics& mat3);
	static void UpdateUniform(GLint location, const Vec4Graphics& vec4);
	static void UpdateUniform(GLint location, const Vec3Graphics& vec3);
	static void UpdateUniform(GLint location, const Vec2Graphics& vec2);
	static void UpdateUniform(GLint location, float f);
	static void UpdateUniform(GLint location, double d);
	static void UpdateUniform(GLint location, int i);
	static void UpdateUniform(GLint location, unsigned int u);
	void UpdateShaderMatrices();

protected:
	virtual void	Resize(int x, int y);

	void			SetTextureRepeating(GLuint target, bool state);

	OGLShader* currentShader;

	Mat4Graphics projMatrix;		//Projection matrix
	Mat4Graphics viewMatrix;		//View matrix
	Mat4Graphics textureMatrix;	//Texture matrix

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context
};
#endif