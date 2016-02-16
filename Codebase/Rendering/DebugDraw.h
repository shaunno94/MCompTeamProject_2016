#pragma once

#include "LinearMath/btIDebugDraw.h"
#include "Renderer.h"

class DebugDraw : public btIDebugDraw
{
public:
	static DebugDraw* Instance();
	static void Context(Renderer* r) { renderContext = r; }
	static void Release();

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void setDebugMode(int debugMode){ mode = debugMode; }
	int	getDebugMode() const { return mode; }

	//Not yet implemented
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
	void reportErrorWarning(const char* warningString) {}
	void draw3dText(const btVector3& location, const char* textString){}

private:
	DebugDraw() {}
	~DebugDraw();

	static Shader* debugShader;
	static GLuint m_glArray;
	static GLuint m_glBuffer;
	static DebugDraw* DebugInstance;
	static const char* dVertex;
	static const char* dFrag;
	static Renderer* renderContext;
	static int mode;
	static vector<btVector3> lines;
};