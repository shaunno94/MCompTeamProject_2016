
#include <iostream>
#include "Helpers/common.h"
#include "Helpers/File.h"
#include "ModelLoader.h"
#include "Rendering/constants.h"

int main() {

	std::cout << "Give filepath starting from " << MODEL_DIR << ":" << LINE_SEPARATOR;
	std::string filepath;
	std::cin >> filepath;
	filepath = MODEL_DIR + filepath;

	string newFilepath = File::RemoveFileExt(filepath) + ".mgl";
	ModelLoader::SaveMeshToMGL(ModelLoader::LoadOBJ(filepath, false), newFilepath);
}