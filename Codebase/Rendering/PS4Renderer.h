#pragma once
#ifdef ORBIS
#include "PS4Buffer.h"
#include "Helpers/common.h"
#include "LightMaterial.h"
#include "PS4Frame.h"

enum CULL
{
	FRONT, BACK
};

enum MemoryLocation
{
	GARLIC,
	ONION,
	GARLIC_MESH,
	MEMORYMAX
};

enum BACK_BUFFERS
{
	BACK_BUFFER1, BACK_BUFFER2, BACK_BUFFER3, MAX_BACK_BUFFER
};

enum FBO
{
	G_BUFFER, LIGHT_BUFFER, SHADOW_BUFFER, MAX_FBO
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

	void UpdateUniform(const shaderResourceLocation& location, const Mat4Graphics& mat4);
	void UpdateUniform(const shaderResourceLocation& location, const Mat3Graphics& mat3);
	void UpdateUniform(const shaderResourceLocation& location, const Vec4Graphics& vec4);
	void UpdateUniform(const shaderResourceLocation& location, const Vec3Graphics& vec3);
	void UpdateUniform(const shaderResourceLocation& location, const Vec2Graphics& vec2);
	void UpdateUniform(const shaderResourceLocation& location, float f);
	void UpdateUniform(const shaderResourceLocation& location, double d);
	void UpdateUniform(const shaderResourceLocation& location, int i);
	void UpdateUniform(const shaderResourceLocation& location, unsigned int u);
	
	unsigned int TextureMemoryUsage(sce::Gnm::Texture& id);
	void SetTexture(const shaderResourceLocation& location, textureHandle& handle);

	sce::Gnmx::GnmxGfxContext* GetGFXContext() { return currentGFXContext; }

protected:	
	void FillBuffers();
	void DrawPointLights();
	void DrawShadow(GameObject* light);
	void DrawShadow2D(GameObject* light);
	void DrawShadowCube(GameObject* light);
	void CombineBuffers();
	void DrawSkyBox();
	void SwapBuffers();
	void InitCMD(PS4Buffer* buffer);
	
	void SetCullFace(CULL c);
	
	Mat4Graphics projMatrix;	//Projection matrix
	Mat4Graphics viewMatrix;	//View matrix
	
	static Renderer* child;
	PS4Shader* currentShader;
	const uint width = 1920;
	const uint height = 1080;

private:	
	//VIDEO SYSTEM VARIABLES
	int videoHandle;	
	int currentGPUBuffer;
	const int _MaxCMDBufferCount = 3;

	//SCREEN BUFFER VARIABLES
	int	currentScreenBuffer;
	int	prevScreenBuffer;
	std::vector<PS4Buffer*> screenBuffers;
	std::vector<PS4Buffer*> offScreenBuffers;
	const uint MAX_TARGETS_PER_BUFFER = 2;

	//Per frame pointers.
	PS4Buffer* currentPS4Buffer;
	sce::Gnmx::GnmxGfxContext*	currentGFXContext;
	PS4Frame* currentFrame;
	PS4Frame* frames;

	//Memory Allocation
	const int _GarlicMemory = (1024 * 1024 * 512);
	const int _OnionMemory = (1024 * 1024 * 256);
	sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORYMAX];

	bool init = false;

	void	InitialiseMemoryAllocators();
	void	InitialiseVideoSystem();
	void	InitialiseGCMRendering();
	void	RegisterTargets(const uint targetIndex, std::vector<PS4Buffer*>& buffer);

	void	DestroyMemoryAllocators();
	void	DestroyVideoSystem();
	void	DestroyGCMRendering();
	
	void SwapCommandBuffer();
	void SwapScreenBuffer();

	GameObject* fullScreenQuad;
	GameObject* skyQuad;

	//FBO Texture locations
	PS4ShaderResourceLocations specularLoc;
	PS4ShaderResourceLocations emissiveLoc;
	PS4ShaderResourceLocations diffuseLoc;
	PS4ShaderResourceLocations shadowLoc;
	PS4ShaderResourceLocations normalLoc;
	PS4ShaderResourceLocations depthLoc;

	//Control objects
	sce::Gnm::Sampler trilinearSampler;
	sce::Gnm::Sampler shadowSampler;
	sce::Gnm::PrimitiveSetup primitiveSetup;
	sce::Gnm::DepthStencilControl dsc;
	sce::Gnm::BlendControl blendControl;
	sce::Gnm::DepthEqaaControl dEqaaControl;
	sce::Gnm::ClipControl cc;
	sce::Gnm::DbRenderControl dbRenderControl;

	//Shadow Projection matrix
	Matrix4Simple shadowProj = Matrix4Simple::Perspective(50.0f, 15000.0f, 1.0f, 45.0f);

	bool previousWasShadow = false;
};
#endif