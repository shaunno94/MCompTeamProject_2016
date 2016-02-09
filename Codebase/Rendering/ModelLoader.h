#pragma once

#include <string>
#include "Mesh.h"
#include <vector>
#include <sstream>
#include "constants.h"

class ModelLoader
{
public:
	// Loads file and returns correct Mesh, buffered or unbuffered
	static Mesh* LoadMGL(const std::string& filePath, bool bufferData = true);

protected:
	// Singleton protection
	ModelLoader() {}
	~ModelLoader() {}

};

