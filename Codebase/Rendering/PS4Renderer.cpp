#ifdef ORBIS
#include "PS4Renderer.h"

Renderer* PS4Renderer::child = nullptr;

PS4Renderer::PS4Renderer()
{

}

PS4Renderer::~PS4Renderer()
{

}

void PS4Renderer::SetTextureFlags(const sce::Gnm::Texture*, unsigned int flags)
{

}

void PS4Renderer::SetCurrentShader(BaseShader* s)
{
	currentShader = static_cast<PS4Shader*>(s);
}

void PS4Renderer::FillBuffers()
{

}

void PS4Renderer::DrawPointLights()
{

}

void PS4Renderer::DrawShadow(GameObject* light)
{

}

void PS4Renderer::CombineBuffers()
{

}

void PS4Renderer::ClearBuffer(bool colour, bool depth, bool stencil)
{
	/*if (colour) 
	{
		Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);

		SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, defaultClearColour);
	}

	if (depth) 
	{
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultDepth);
	}

	if (stencil && currentPS4Buffer->depthTarget.getStencilReadAddress()) 
	{
		int defaultStencil = 0;
		SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultStencil);
	}*/
}

void PS4Renderer::SwapBuffers()
{

}

void PS4Renderer::SwapCommandBuffer()
{

}

void PS4Renderer::SetCullFace(CULL c)
{

}
#endif