#ifndef ORBIS
#include "DebugDrawOGL.h"
#include "OGLShader.h"

const char* DebugDrawOGL::dFrag = SHADER_DIR"Debug/Fragment.glsl";
const char* DebugDrawOGL::dVertex = SHADER_DIR"Debug/Vertex.glsl";
OGLShader*	DebugDrawOGL::debugShader = nullptr;
GLuint DebugDrawOGL::m_glArray = 0;
GLuint DebugDrawOGL::m_glBuffer = 0;

void DebugDrawOGL::Instance()
{
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

void DebugDrawOGL::DrawDebugLines(std::vector<btVector3>& lines, Renderer* context)
{
	glBindVertexArray(m_glArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(btVector3), lines.data(), GL_DYNAMIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(btVector3), lines.data());

	context->SetCurrentShader(debugShader);
	context->UpdateShaderMatrices();
	glDrawArrays(GL_LINES, 0, lines.size());

	glUseProgram(0);
	glBindVertexArray(0);
}

void DebugDrawOGL::Release()
{
	glDeleteVertexArrays(1, &m_glArray);
	glDeleteBuffers(1, &m_glBuffer);
	delete debugShader;
}
#endif