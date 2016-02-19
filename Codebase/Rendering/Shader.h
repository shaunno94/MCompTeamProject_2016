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

};