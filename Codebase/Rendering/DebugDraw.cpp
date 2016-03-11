#include "DebugDraw.h"
#include "DebugDrawPS4.h"
#include "DebugDrawOGL.h"

DebugDraw* DebugDraw::DebugInstance = nullptr;
std::vector<btVector3> DebugDraw::lines = std::vector<btVector3>();
Renderer* DebugDraw::renderContext = nullptr;
int DebugDraw::mode = DebugDrawModes::DBG_DrawWireframe;

DebugDraw* DebugDraw::Instance()
{
	if (!DebugInstance)
	{
		DebugInstance = new DebugDraw();

#ifdef ORBIS
		DebugDrawPS4::Instance();
#else
		DebugDrawOGL::Instance();
#endif
	}
	return DebugInstance;
}

void DebugDraw::Release()
{
	if (DebugInstance) {
		delete DebugInstance;
		DebugInstance = nullptr;
	}
}

DebugDraw::~DebugDraw()
{
#ifdef ORBIS
	DebugDrawPS4::Release();
#else
	DebugDrawOGL::Release();
#endif
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	lines.push_back(to);
	lines.push_back(from);
	Material::s_LastMaterialInUse = nullptr;
}	

void DebugDraw::RenderLine()
{
#ifdef ORBIS
	DebugDrawPS4::DrawDebugLines(lines, renderContext);
#else
	DebugDrawOGL::DrawDebugLines(lines, renderContext);
#endif
	lines.clear();
}