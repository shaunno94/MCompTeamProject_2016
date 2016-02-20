#pragma once
#ifdef ORBIS
#include "Helpers/common.h"
#include <string>
#include <vector>
#include "LightMaterial.h"
#include "Math/nclglMath.h"
#include "PS4Shader.h"

enum CULL
{
	FRONT, BACK
};

enum MemoryLocation
{
	GARLIC,
	ONION,
	MEMORYMAX
};

struct PS4ScreenBuffer 
{
	sce::Gnm::RenderTarget		colourTarget;
	sce::Gnm::DepthRenderTarget depthTarget;

	//uint width;
	//uint height;
	//sce::Gnmx::Toolkit::Timers   m_timers; ?????
};

class Renderer;
class GameObject;

class PS4Renderer
{
public:
	PS4Renderer();
	~PS4Renderer();

	void UpdateShaderMatrices(){}
	bool HasInitialised() const { return init; }
	void SetTextureFlags(const sce::Gnm::Texture*, unsigned int flags);
	void SetCurrentShader(BaseShader* s);

	static void UpdateUniform(int location, const Mat4Graphics& mat4){}
	static void UpdateUniform(int location, const Mat3Graphics& mat3){}
	static void UpdateUniform(int location, const Vec4Graphics& vec4){}
	static void UpdateUniform(int location, const Vec3Graphics& vec3){}
	static void UpdateUniform(int location, const Vec2Graphics& vec2){}
	static void UpdateUniform(int location, float f){}
	static void UpdateUniform(int location, double d){}
	static void UpdateUniform(int location, int i){}
	static void UpdateUniform(int location, unsigned int u){}
	
	unsigned int TextureMemoryUsage(const sce::Gnm::Texture* id){ return 0; }
protected:	
	void FillBuffers();
	void DrawPointLights();
	void DrawShadow(GameObject* light);
	void CombineBuffers();
	void SwapBuffers();
	void SwapCommandBuffer();
	void SetCullFace(CULL c);
	
	Mat4Graphics projMatrix;	//Projection matrix
	Mat4Graphics viewMatrix;	//View matrix
	Mat4Graphics textureMatrix;	//Texture matrix
	
	static Renderer* child;
	PS4Shader* currentShader;

	bool init = false;

private:
	void	InitialiseMemoryAllocators();
	void	InitialiseVideoSystem();
	void	InitialiseGCMRendering();

	void	DestroyMemoryAllocators();
	void	DestroyVideoSystem();
	void	DestroyGCMRendering();

	void	SetRenderBuffer(PS4ScreenBuffer* buffer, bool clearColour, bool clearDepth, bool clearStencil);
	void	ClearBuffer(bool colour, bool depth, bool stencil);

	PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
};
#endif