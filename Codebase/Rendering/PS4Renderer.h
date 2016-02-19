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
class PS4Renderer
{
public:
	bool HasInitialised() const { return init; }

protected:
	
	void UpdateShaderMatrices();
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void SwapBuffers();
	void SetCullFace(CULL c);
	
	Mat4Graphics projMatrix;	//Projection matrix
	Mat4Graphics viewMatrix;	//View matrix
	Mat4Graphics textureMatrix;	//Texture matrix
	
	static Renderer* child;
	PS4Shader* currentShader;

	bool init = false;
};
#endif