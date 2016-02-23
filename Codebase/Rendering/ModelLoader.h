#pragma once

#include <string>

#ifndef ORBIS
#include "OGLMesh.h"
#else
#include "PS4Mesh.h"
#endif
#include "constants.h"

/// @ingroup Rendering
/// <summary>
/// Holds functionality for loading a mesh from the hard-drive.
/// </summary>
static class ModelLoader
{
public:
	// Loads file and returns correct Mesh, buffered or unbuffered
	static Mesh* LoadMGL(const std::string& filePath, bool bufferData = true);

protected:
	// Singleton protection
	ModelLoader() {}
	~ModelLoader() {}

};

