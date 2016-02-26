#pragma once
#ifdef ORBIS
#include <gnm.h>
#include <gnmx\fetchshaderhelper.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>

#include "Helpers/common.h"
#include "LightMaterial.h"
#include "PS4Shader.h"
#include "PS4Frame.h"

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
class PS4Mesh;

class PS4Renderer : public PS4Memory
{
public:
	PS4Renderer();
	~PS4Renderer();

	void UpdateShaderMatrices();
	bool HasInitialised() const { return init; }
	void SetTextureFlags(textureHandle&, unsigned int flags);
	void SetCurrentShader(BaseShader* s);

	void UpdateUniform(int location, const Mat4Graphics& mat4);
	void UpdateUniform(int location, const Mat3Graphics& mat3);
	void UpdateUniform(int location, const Vec4Graphics& vec4);
	void UpdateUniform(int location, const Vec3Graphics& vec3);
	void UpdateUniform(int location, const Vec2Graphics& vec2);
	void UpdateUniform(int location, float f);
	void UpdateUniform(int location, double d);
	void UpdateUniform(int location, int i);
	void UpdateUniform(int location, unsigned int u);
	
	unsigned int TextureMemoryUsage(sce::Gnm::Texture id){ return 0; }
	void SetTexture(unsigned int id, textureHandle& handle);

	sce::Gnmx::GnmxGfxContext* GetGFXContext() { return currentGFXContext; }

protected:	
	void FillBuffers();
	void DrawPointLights();
	void DrawShadow(GameObject* light);
	void CombineBuffers();
	void SwapBuffers();
	
	void SetCullFace(CULL c);
	
	Mat4Graphics projMatrix;	//Projection matrix
	Mat4Graphics viewMatrix;	//View matrix
	
	static Renderer* child;
	PS4Shader* currentShader;

	//Per frame pointers.
	PS4ScreenBuffer* currentPS4Buffer;  //Pointer to whichever buffer we're currently using...
	sce::Gnmx::GnmxGfxContext*	currentGFXContext;
	PS4Frame* currentFrame;
	PS4Frame* frames;
	int framesSubmitted;

	int currentGPUBuffer;
	const int _MaxCMDBufferCount = 3;

	//VIDEO SYSTEM VARIABLES
	int videoHandle;

	//SCREEN BUFFER VARIABLES
	const int _bufferCount = 3;	//How many screen buffers should we have
	int	currentScreenBuffer;
	int	prevScreenBuffer;
	PS4ScreenBuffer** screenBuffers;	//Pointer to our screen buffers

	//Memory Allocation
	const int _GarlicMemory = (1024 * 1024 * 512);
	const int _OnionMemory = (1024 * 1024 * 256);
	sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORYMAX];

	sce::Gnm::Sampler trilinearSampler;
	sce::Gnm::PrimitiveSetup primitiveSetup;
	sce::Gnm::DepthStencilControl dsc;
	PS4Mesh* mesh;
	PS4Shader* shader;
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
	
	void SwapCommandBuffer();
	void SwapScreenBuffer();

	PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
};
#endif