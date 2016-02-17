#include "DebugDraw.h"

DebugDraw* DebugDraw::DebugInstance = nullptr;
const char* DebugDraw::dFrag = SHADER_DIR"Debug/Fragment.glsl";
const char* DebugDraw::dVertex = SHADER_DIR"Debug/Vertex.glsl";
OGLShader* DebugDraw::debugShader = nullptr;
vector<btVector3> DebugDraw::lines = vector<btVector3>();
GLuint DebugDraw::m_glArray = 0;
GLuint DebugDraw::m_glBuffer =  0;
Renderer* DebugDraw::renderContext = nullptr;
int DebugDraw::mode = DebugDrawModes::DBG_DrawWireframe;

DebugDraw* DebugDraw::Instance()
{
	if (!DebugInstance)
	{
		DebugInstance = new DebugDraw();
		glGenVertexArrays(1, &m_glArray);	
		glGenBuffers(1, &m_glBuffer);
		debugShader = new OGLShader(dVertex, dFrag);

		glBindVertexArray(m_glArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(btVector3), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(btVector3), 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);
		glBindVertexArray(0);
	}
	return DebugInstance;
}

void DebugDraw::Release()
{
	delete DebugInstance;
}

DebugDraw::~DebugDraw()
{
	glDeleteVertexArrays(1, &m_glArray);
	glDeleteBuffers(1, &m_glBuffer);
	delete debugShader;
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	lines.clear();
	lines.push_back(to);
	lines.push_back(from);

	glBindVertexArray(m_glArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(btVector3), lines.data());

	renderContext->SetCurrentShader(debugShader);
	renderContext->UpdateShaderMatrices();
	glDrawArrays(GL_LINES, 0, lines.size());
	
	glUseProgram(0);
	glBindVertexArray(0);
	Material::s_LastMaterialInUse = nullptr;
}	

/*glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer[COLOUR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(btVector3), &color, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOUR, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOUR);*/