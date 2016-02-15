#pragma once

//#include "SFML/Window.hpp"
//#include "SFML/OpenGL.hpp"

#include "Helpers/common.h"

#include <string>
#include <fstream>
#include <vector>

#include "GL/glew.h"
#include "GL/wglew.h"
#include "Math/nclglMath.h"
#include "Window.h"
//#include "light.h"

#include "Shader.h"		//Students make this file...
#include "Mesh.h"		//And this one...

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew-1.13.0/lib/Release/Win32/glew32s.lib")

/*
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#ifdef _DEBUG
#pragma comment(lib, "sfml-system-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#else
#pragma comment(lib, "sfml-system-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#endif
*/

#include <vector>

#ifdef _DEBUG
#define GL_BREAKPOINT glUniform4uiv(0,0,0);//Invalid, but triggers gdebugger ;)
#else
#define GL_BREAKPOINT //
#endif

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

/// @ingroup Rendering
/// <summary>
///
/// </summary>
enum DebugDrawMode
{
	DEBUGDRAW_ORTHO,
	DEBUGDRAW_PERSPECTIVE
};

/// @ingroup Rendering
/// <summary>
///
/// </summary>
struct DebugDrawData
{
	std::vector<Vec3Graphics> lines;
	std::vector<Vec3Graphics> colours;

	GLuint array;
	GLuint buffers[2];

	DebugDrawData();
	void Draw();

	~DebugDrawData()
	{
		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(2, buffers);
	}

	inline void Clear()
	{
		lines.clear();
		colours.clear();
	}

	inline void AddLine(const Vec3Graphics& from, const Vec3Graphics& to, const Vec3Graphics& fromColour, const Vec3Graphics& toColour)
	{
		lines.push_back(from);
		lines.push_back(to);

		colours.push_back(fromColour);
		colours.push_back(toColour);
	}
};


class Shader;

/// @ingroup Rendering
/// <summary>
/// OpenGL specific functionality for the Renderer.
/// </summary>
class OGLRenderer
{
public:
	friend class Window;
	OGLRenderer(Window& parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene(float msec) = 0;
	virtual void	UpdateScene(float msec);
	void			SwapBuffers();

	bool			HasInitialised() const;

	static void		DrawDebugLine(DebugDrawMode mode, const Vec3Graphics& from, const Vec3Graphics& to, const Vec3Graphics& fromColour = Vec3Graphics(1, 1, 1), const Vec3Graphics& toColour = Vec3Graphics(1, 1, 1));
	static void		DrawDebugBox(DebugDrawMode mode, const Vec3Graphics& at, const Vec3Graphics& scale, const Vec3Graphics& colour = Vec3Graphics(1, 1, 1));
	static void		DrawDebugCross(DebugDrawMode mode, const Vec3Graphics& at, const Vec3Graphics& scale, const Vec3Graphics& colour = Vec3Graphics(1, 1, 1));
	static void		DrawDebugCircle(DebugDrawMode mode, const Vec3Graphics& at, const float radius, const Vec3Graphics& colour = Vec3Graphics(1, 1, 1));
	static void		DrawDebugMatrix(const Mat3Physics& m, const Vec3Physics& position);

	void			SetAsDebugDrawingRenderer()
	{
		debugDrawingRenderer = this;
	}

	Shader*			GetCurrentShader() const
	{
		return currentShader;
	}
	void			SetCurrentShader(Shader* s);

	static void UpdateUniform(GLint location, const Mat4Graphics& mat4);
	static void UpdateUniform(GLint location, const Mat3Graphics& mat3);
	static void UpdateUniform(GLint location, const Vec4Graphics& vec4);
	static void UpdateUniform(GLint location, const Vec3Graphics& vec3);
	static void UpdateUniform(GLint location, const Vec2Graphics& vec2);
	static void UpdateUniform(GLint location, float f);
	static void UpdateUniform(GLint location, double d);
	static void UpdateUniform(GLint location, int i);
	static void UpdateUniform(GLint location, unsigned int u);
	void			UpdateShaderMatrices();

protected:
	virtual void	Resize(int x, int y);

	void			SetTextureRepeating(GLuint target, bool state);

	//void			SetShaderLight(const Light &l);

	void			DrawDebugPerspective(Mat4Graphics* matrix = 0);
	void			DrawDebugOrtho(Mat4Graphics* matrix = 0);

	Shader* currentShader;


	Mat4Graphics projMatrix;		//Projection matrix
	Mat4Graphics viewMatrix;		//View matrix
	Mat4Graphics textureMatrix;	//Texture matrix

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context

	static DebugDrawData* orthoDebugData;
	static DebugDrawData* perspectiveDebugData;

	static OGLRenderer*	  debugDrawingRenderer;
	static Shader*		  debugDrawShader;



#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLuint source, GLuint type, GLuint id, GLuint severity,
	                                   int length, const char* message, void* userParam);
#endif

	static bool	drawnDebugOrtho;
	static bool	drawnDebugPerspective;

};