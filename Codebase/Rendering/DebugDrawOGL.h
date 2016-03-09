#ifndef ORBIS
#pragma once
#include "DebugDraw.h"

class DebugDrawOGL
{
	friend class DebugDraw;
public:

private:
	DebugDrawOGL() {}
	~DebugDrawOGL() {}

	static void Instance();
	static void Release();
	static void DrawDebugLines(std::vector<btVector3>& lines, Renderer* context);

	static OGLShader* debugShader;
	static GLuint m_glArray;
	static GLuint m_glBuffer;
	static const char* dVertex;
	static const char* dFrag;
};
#endif