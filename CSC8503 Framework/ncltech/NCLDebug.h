/******************************************************************************
Class: NCLDebug
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description:
Encapsulates lots of globally accessible methods to output information to the screen.
Should be able to draw thin/thick lines, circles, text, triangles etc from anywhere in your
program.

This is rendered each frame from within the render section of the scene itself.


Note: Both 'point_radius' (DrawPoint) and 'line_width' (DrawThickLine) are in world-space coordinates
 while 'font_size' (All Text Functions) is in terms of screen pixels - the same as microsoft word etc.


		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <nclgl\Shader.h>
#include <vector>
#include <mutex>
#include "Math/nclglMath.h"

#define MAX_LOG_SIZE		25
#define LOG_TEXT_SIZE  		12.0f
#define STATUS_TEXT_SIZE	16.0f

enum TextAlignment {
	TEXTALIGN_LEFT,
	TEXTALIGN_RIGHT,
	TEXTALIGN_CENTRE
};

struct LogEntry {
	Vec3Graphics colour;
	std::string text;
};

class NCLDebug {
	friend class Scene;
	friend class MyScene;
 public:

	static void DrawPoint(const Vec3Physics& pos, float point_radius, const Vec4Graphics& colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawThickLine(const Vec3Physics& start, const Vec3Physics& end, float line_width, const Vec4Graphics& colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawHairLine(const Vec3Physics& start, const Vec3Physics& end, const Vec4Graphics& colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawMatrix(const Mat4Physics& transform_mtx);
	static void DrawMatrix(const Mat3Physics& rotation_mtx, const Vec3Physics& position);

	static void DrawTriangle(const Vec3Physics& v0, const Vec3Physics& v1, const Vec3Physics& v2, const Vec4Graphics& colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawPolygon(int n_verts, const Vec3Physics* verts, const Vec4Graphics& colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawTextClipSpace(const Vec4Physics& pos, const float font_size, const string& text, const TextAlignment alignment = TEXTALIGN_LEFT, const Vec4Graphics colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawTextA(const Vec3Physics& pos, const float font_size, const string& text, const TextAlignment alignment = TEXTALIGN_LEFT, const Vec4Graphics colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawTextF(const Vec3Physics& pos, const float font_size, const TextAlignment alignment, const Vec4Graphics colour, const string text, ...); ///See "printf" for usuage manual


	static void AddStatusEntry(const Vec4Graphics& colour, const std::string text, ...); ///See "printf" for usuage manual
	static void Log(const Vec3Graphics& colour, const std::string text, ...); ///See "printf" for usuage manual

	static void SwapBuffers() {
		g_WriteBufferIndex = (g_WriteBufferIndex + 1) % 2;
	}
	static volatile bool g_NewData;

	static void LoadShaders();
	static void ReleaseShaders();

 protected:
	static unsigned int g_WriteBufferIndex;

	//Called by Scene class
	static void ClearDebugLists();
	static void SortDebugLists();
	static void DrawDebugLists();

	static void SetDebugDrawData(const Mat4Graphics& projViewMatrix, const Vec3Physics& camera_pos) {
		m_ProjView = projViewMatrix;
		m_CameraPosition = camera_pos;
	}

 protected:
	static Vec3Physics	m_CameraPosition;
	static Mat4Physics	m_ProjView;

	static int m_NumStatusEntries;
	static std::vector<LogEntry> m_LogEntries;
	static int m_LogEntriesOffset;

	static std::vector<Vec4Graphics> m_Points[2];
	static std::vector<Vec4Graphics> m_HairLines[2];
	static std::vector<Vec4Graphics> m_ThickLines[2];
	static std::vector<Vec4Graphics> m_Tris[2];
	static std::vector<Vec4Graphics> m_Characters;

	static Shader*	m_pShaderPoints;
	static Shader*	m_pShaderLines;
	static Shader*	m_pShaderHairLines;
	static Shader*	m_pShaderText;

	static GLuint	m_glArray, m_glBuffer;
	static GLuint	m_glFontTex;


	static std::mutex m_DebugMutex;
};