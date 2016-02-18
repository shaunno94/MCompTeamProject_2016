#pragma once
#ifndef ORBIS

#include "OGLShader.h"
class Shader : public OGLShader
#else
#include "PS4Shader.h"
class Shader : public PS4Shader
#endif
{
public:
	Shader(string VERTEX, string FRAG, string GEO = "", string TCS_HULL = "", string TES_DOMAIN = "");
	Shader(int i);
protected:

};