#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "Renderer.h"

class DebugDraw : public btIDebugDraw
{
public:
	static DebugDraw* Instance();
	static void Release();
	static void Context(Renderer* r) { renderContext = r; }

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void setDebugMode(int debugMode)	{ mode = debugMode; }
	int	getDebugMode() const			{ return mode; }
	void RenderLine();

	//Not yet implemented
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
	void reportErrorWarning(const char* warningString) {}
	void draw3dText(const btVector3& location, const char* textString){}

protected:
	DebugDraw() {}
	~DebugDraw();

	static DebugDraw* DebugInstance;
	static Renderer* renderContext;
	static int mode;
	static std::vector<btVector3> lines;
};