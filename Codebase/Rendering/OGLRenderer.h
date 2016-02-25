#ifndef ORBIS
#pragma once
#include "Helpers/common.h"
#include <string>
#include <vector>

#include "Dependencies/glew-1.13.0/include/GL/glew.h"
#include "Dependencies/glew-1.13.0/include/GL/wglew.h"
#include "Math/nclglMath.h"
#include "Window.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew-1.13.0/lib/Release/Win32/glew32s.lib")

#include "LightMaterial.h"
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
class GameObject;
class Renderer;
enum CULL
{
	FRONT, BACK
};
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

	OGLShader*	GetCurrentShader() const
	{
		return currentShader;
	}
	void SetCurrentShader(BaseShader* s);

	void UpdateUniform(GLint location, const Mat4Graphics& mat4);
	void UpdateUniform(GLint location, const Mat3Graphics& mat3);
	void UpdateUniform(GLint location, const Vec4Graphics& vec4);
	void UpdateUniform(GLint location, const Vec3Graphics& vec3);
	void UpdateUniform(GLint location, const Vec2Graphics& vec2);
	void UpdateUniform(GLint location, float f);
	void UpdateUniform(GLint location, double d);
	void UpdateUniform(GLint location, int i);
	void UpdateUniform(GLint location, unsigned int u);
	void UpdateShaderMatrices();
	void SetTextureFlags(unsigned int id, unsigned int flags);
	unsigned int TextureMemoryUsage(unsigned int id);
	void SetTexture(unsigned int id, textureHandle handle);

protected:
	virtual void	Resize(int x, int y);

	void			SetTextureRepeating(GLuint target, bool state);
	void SetCullFace(enum CULL c) { glCullFace(c == FRONT ? GL_FRONT : GL_BACK); };

	OGLShader* currentShader;

	Mat4Graphics projMatrix;		//Projection matrix
	Mat4Graphics viewMatrix;		//View matrix
	Mat4Graphics textureMatrix;	//Texture matrix

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context

	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void initFBO();
	void GenerateScreenTexture(GLuint & into, bool depth = false);
	void DrawShadow(GameObject* light);
	void DrawShadow2D(GameObject* light);
	void DrawShadowCube(GameObject* light);

	GLuint bufferFBO; // FBO for G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; // FBO for lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	GLuint shadowFBO;
	GLuint cubeShadowFBO;
	GLuint shadowTex2D; //stores depths for shadow calulations
	GLuint shadowTexCube; //stores depths for shadow calulations

	Vec3Graphics directions[6];//TODO - should be constants
	Vec3Graphics up[6];

	GameObject* quad;
	static Renderer* child;
};
#endif